package decode

import "sync"

var (
	maxSize = 1024 // 20k

)

type bufferPool struct {
	bp      *sync.Pool
	maxSize int
}

var bytePool *bufferPool

func init() {
	bytePool = newBufferPool(maxSize)
}



func newBufferPool(maxSize int) *bufferPool {
	return &bufferPool{
		bp: &sync.Pool{
			New: func() interface{} {
				b := make([]byte, maxSize)
				return b
			},
		},
		maxSize: maxSize,
	}
}

func (bpool *bufferPool) get() []byte {
	return bpool.bp.Get().([]byte)
}

func (bpool *bufferPool) put(buf []byte) {
	if cap(buf) > maxSize {
		return
	}

	// reset length to 0
	bpool.bp.Put(buf[:0])
}
