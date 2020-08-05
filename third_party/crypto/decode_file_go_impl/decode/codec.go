package decode

/*
#cgo CFLAGS: -I../../third_party/micro-ecc-master

#include "../../third_party/micro-ecc-master/uECC.h"
#include "../../third_party/micro-ecc-master/uECC.c"
extern int uECC_shared_secret(const uint8_t *public_key, const uint8_t *private_key, uint8_t *secret, uECC_Curve curve);
extern uECC_Curve uECC_secp256k1(void);
*/
import "C"
import "unsafe"

import (
	"bytes"
	"compress/flate"
	"encoding/binary"
	"encoding/hex"
	"errors"
	"fmt"
	"io"
)

const (
	MAGIC_NO_COMPRESS_START          = 0x03
	MAGIC_NO_COMPRESS_START1         = 0x06
	MAGIC_NO_COMPRESS_NO_CRYPT_START = 0x08
	MAGIC_COMPRESS_START             = 0x04
	MAGIC_COMPRESS_START1            = 0x05
	MAGIC_COMPRESS_START2            = 0x07
	MAGIC_COMPRESS_NO_CRYPT_START    = 0x09

	MAGIC_END = 0x00

	PRIV_KEY = "145aa7717bf9745b91e9569b80bbf1eedaa6cc6cd0e26317d810e35710f44cf8"
	PUB_KEY  = "572d1e2710ae5fbca54c76a382fdd44050b3a675cb2bf39feebe85ef63d947aff0fa4943f1112e8b6af34bebebbaefa1a0aae055d9259b89a1858f7cc9af9df1"
)

func EccSharedSecret(public_key []byte, private_key []byte) ([32]byte, error) {
	var secret [32]byte
	fmt.Println(len(public_key))
	var ret = C.uECC_shared_secret((*C.uchar)(unsafe.Pointer(&public_key[0])), (*C.uchar)(unsafe.Pointer(&private_key[0])), (*C.uchar)(unsafe.Pointer(&secret[0])), C.uECC_secp256k1())
	if ret == 0 {
		return secret, errors.New("public key and private key is err")
	}
	return secret, nil
}

func Decode(src []byte) ([]byte, error) {
	offset := GetLogStartPos(src, 2)
	if offset == -1 {
		return nil, errors.New("offset == -1")
	}
	dest := BufGet()
	defer BufPut(dest)
	for {
		offset = DecodeBuffer(src, dest, offset)
		if offset == -1 {
			break
		}
	}

	return dest.Bytes(), nil
}

func DecodeBuffer(src []byte, dest *bytes.Buffer, offset int) int {
	if offset >= len(src) {
		return -1
	}

	if !IsGoodLogBuffer(src, offset, 1) {
		fixpos := GetLogStartPos(src[offset:], 1)
		if fixpos == -1 {
			return -1
		} else {
			//golog.Error("DecodeBuffer", "err", "decode error")
			offset += fixpos
		}
	}

	var crypt_key_len int
	magic_start := src[offset]
	if MAGIC_NO_COMPRESS_START == magic_start || MAGIC_COMPRESS_START == magic_start || MAGIC_COMPRESS_START1 == magic_start {
		crypt_key_len = 4
	} else if MAGIC_COMPRESS_START2 == magic_start ||
		MAGIC_NO_COMPRESS_START1 == magic_start ||
		MAGIC_NO_COMPRESS_NO_CRYPT_START == magic_start ||
		MAGIC_COMPRESS_NO_CRYPT_START == magic_start {
		crypt_key_len = 64
	} else {
		//golog.Error("DecodeBuffer", "err", fmt.Sprintf("%d != MAGIC_NUM_START", magic_start))
		return -1
	}

	headerLen := 1 + 2 + 1 + 1 + 4 + crypt_key_len

	var length uint32
	startLen := offset + headerLen - 4 - crypt_key_len
	endLen := offset + headerLen - crypt_key_len

	if endLen > len(src) {
		//golog.Warn("IsGoodLogBuffer endLen > len(src)", "warn", " index out of range")
		return -1
	}
	err := binary.Read(bytes.NewBuffer(src[startLen:endLen]), binary.LittleEndian, &length)
	if err != nil {
		//golog.Error("IsGoodLogBuffer binary.Read", "err", err)
		return -1
	}

	var seq uint16
	startLen = offset + headerLen - 4 - crypt_key_len - 2 - 2
	endLen = offset + headerLen - 4 - crypt_key_len - 2
	if endLen > len(src) {
		//golog.Warn("IsGoodLogBuffer endLen > len(src)", "warn", " index out of range")
		return -1
	}
	err = binary.Read(bytes.NewBuffer(src[startLen:endLen]), binary.LittleEndian, &seq)
	if err != nil {
		//golog.Error("IsGoodLogBuffer binary.Read", "err", err)
		return -1
	}
	//var lastseq uint16 = 0
	//if seq != 0 && seq != 1 && lastseq != 0 && seq != (lastseq + 1) {
	//
	//}
	//
	//if seq != 0 {
	//	lastseq = seq
	//}
	var tmpbuffer = make([]byte, length)
	tmpbuffer = src[offset+headerLen : offset+headerLen+int(length)]

	var data []byte
	switch {
	case MAGIC_NO_COMPRESS_START1 == src[offset]:
	case MAGIC_COMPRESS_START2 == src[offset]:
		privKeyBuf, err := hex.DecodeString(PRIV_KEY)
		if err != nil {
			//golog.Error("DecodeBuffer", "msg", "hex.DecodeString(PRIV_KEY)", "err", err)
			return offset + headerLen + int(length) + 1
		}

		xstart := offset + headerLen - crypt_key_len
		xend := crypt_key_len + xstart
		if xstart > xend || xend > len(src) {
			//golog.Error("pubkeyX start>end || xend > len(src)", "start", xstart, "end", xend, "ret", offset+headerLen+int(length)+1)
			return offset + headerLen + int(length) + 1
		}

		pubkeyBuf := src[xstart:xend]

		teaKeyS, err := EccSharedSecret(pubkeyBuf, privKeyBuf)
		if err != nil {
			//golog.Error("DecodeBuffer tea_decrypt(tmpbuffer,teaKey)", "err", err)
			return offset + headerLen + int(length) + 1
		}
		teaKey := teaKeyS[:]
		buf, err := tea_decrypt(tmpbuffer, teaKey)
		if err != nil {
			//golog.Error("DecodeBuffer tea_decrypt(tmpbuffer,teaKey)", "err", err)
			return offset + headerLen + int(length) + 1
		}

		data, err = zlibUnCompress(buf)
		if err != nil {
			//golog.Error("DecodeBuffer zlibUnCompress(buf)", "err", err)
		}

	case MAGIC_COMPRESS_START == src[offset] || MAGIC_COMPRESS_NO_CRYPT_START == src[offset]:
		data, err = zlibUnCompress(tmpbuffer)
	case MAGIC_COMPRESS_START1 == src[offset]:
		var decompress_data []byte
		for len(tmpbuffer) > 0 {
			var single_log_len uint16
			startLen = 0
			endLen = 2
			if endLen > len(tmpbuffer) {
				//golog.Error("IsGoodLogBuffer endLen > len(tmpbuffer)", "warn", " index out of range")
				return -1
			}
			err = binary.Read(bytes.NewBuffer(tmpbuffer[startLen:endLen]), binary.LittleEndian, &single_log_len)
			if err != nil {
				//golog.Error("IsGoodLogBuffer binary.Read", "err", err)
				return -1
			}

			if int(single_log_len)+2 > len(tmpbuffer) {
				return -1
			}
			decompress_data = append(decompress_data, tmpbuffer[2:int(single_log_len)+2]...)

			tmpbuffer = tmpbuffer[single_log_len+2:]
		}

		data, err = zlibUnCompress(decompress_data)

	}
	dest.Write(data)
	return offset + headerLen + int(length) + 1
}

func GetLogStartPos(src []byte, count int) int {
	offset := 0
	for {
		if offset >= len(src) {
			break
		}

		if (MAGIC_NO_COMPRESS_START == src[offset] ||
			MAGIC_NO_COMPRESS_START1 == src[offset] ||
			MAGIC_COMPRESS_START == src[offset] ||
			MAGIC_COMPRESS_START1 == src[offset] ||
			MAGIC_COMPRESS_START2 == src[offset] ||
			MAGIC_COMPRESS_NO_CRYPT_START == src[offset] ||
			MAGIC_NO_COMPRESS_NO_CRYPT_START == src[offset]) && IsGoodLogBuffer(src, offset, count) {
			return offset
		}
		offset += 1
	}

	return -1
}

func IsGoodLogBuffer(src []byte, offset, count int) bool {
	var crypt_key_len int
	if offset == len(src) {
		return true
	}
	magic_start := src[offset]
	if MAGIC_NO_COMPRESS_START == magic_start || MAGIC_COMPRESS_START == magic_start || MAGIC_COMPRESS_START1 == magic_start {
		crypt_key_len = 4

	} else if MAGIC_COMPRESS_START2 == magic_start || MAGIC_NO_COMPRESS_START1 == magic_start || MAGIC_NO_COMPRESS_NO_CRYPT_START == magic_start || MAGIC_COMPRESS_NO_CRYPT_START == magic_start {
		crypt_key_len = 64
	} else {
		return false
	}

	headerLen := 1 + 2 + 1 + 1 + 4 + crypt_key_len
	if offset+headerLen+1+1 > len(src) {
		return false
	}

	var length uint32
	startLen := offset + headerLen - 4 - crypt_key_len
	endLen := offset + headerLen - crypt_key_len
	if endLen > len(src) {
		return false
	}

	err := binary.Read(bytes.NewBuffer(src[startLen:endLen]), binary.LittleEndian, &length)
	if err != nil {
		return false
	}
	//log.InfoKV("msg","offset + headerLen + int(length)","len",offset + headerLen + int(length),"offset",offset,"headerLen",headerLen,"int(length)",int(length),"crypt_key_len",crypt_key_len)
	if offset+headerLen+int(length) >= len(src) {
		return false
	}

	if MAGIC_END != src[offset+headerLen+int(length)] {
		return false
	}
	if count <= 1 {
		return true
	}

	return IsGoodLogBuffer(src, offset+headerLen+int(length)+1, count-1)
}

func zlibUnCompress(compressSrc []byte) ([]byte, error) {
	b := bytes.NewReader(compressSrc)
	flateReader := flate.NewReader(b)
	defer flateReader.Close()

	buf := BufGet()
	byteBuf := ByteGet()
	defer func() {
		BufPut(buf)
		BytePut(byteBuf)
	}()
	for {
		n, err := flateReader.Read(byteBuf)
		if n == 0 || err == io.EOF {
			break
		}

		if err != nil {
			break
		}

		_, err = buf.Write(byteBuf[:n])
		if err != nil {
			continue
		}
	}

	return buf.Bytes(), nil
}

var (
	OP    int = 0xffffffff
	DELTA int = 0x9E3779B9
)

func tea_decipher(v, k []byte) ([]byte, error) {
	var v0, v1 int
	var k1, k2, k3, k4 int
	var S = (DELTA << 4) & OP

	if 8 > len(v) || 16 > len(k) {
		return nil, errors.New("index out of range")
	}

	v0 = Byte2Int(v[:4])
	v1 = Byte2Int(v[4:8])
	k1 = Byte2Int(k[0:4])
	k2 = Byte2Int(k[4:8])
	k3 = Byte2Int(k[8:12])
	k4 = Byte2Int(k[12:16])

	for i := 0; i < 16; i++ {
		v1 = (v1 - (((v0 << 4) + k3) ^ (v0 + S) ^ ((v0 >> 5) + k4))) & OP
		v0 = (v0 - (((v1 << 4) + k1) ^ (v1 + S) ^ ((v1 >> 5) + k2))) & OP
		S = (S - DELTA) & OP
	}

	var data []byte
	data = append(data, Int2Byte(v0)...)
	data = append(data, Int2Byte(v1)...)
	return data, nil

}

func Int2Byte(data int) (ret []byte) {
	ret = make([]byte, 4)
	var tmp int = 0xff
	var index uint = 0
	for index = 0; index < uint(4); index++ {
		ret[index] = byte((tmp << (index * 8) & data) >> (index * 8))
	}
	return ret
}

func Byte2Int(data []byte) int {
	var ret int = 0
	var lenData int = len(data)
	var i uint = 0
	for i = 0; i < uint(lenData); i++ {
		ret = ret | (int(data[i]) << (i * 8))
	}
	return ret
}

func tea_decrypt(v, k []byte) ([]byte, error) {
	num := len(v) / 8 * 8
	var ret []byte
	for i := 0; i < num; {
		buf, err := tea_decipher(v[i:i+8], k)
		if err != nil {
			return nil, err
		}
		ret = append(ret, buf...)
		i = i + 8
	}
	ret = append(ret, v[num:]...)
	return ret, nil
}
