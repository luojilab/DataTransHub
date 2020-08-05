package decode

import (
	"bytes"
	"sync"
)

var sp sync.Pool

func init() {
	sp.New = func() interface{} {
		return new(bytes.Buffer)
	}
}

func BufGet() *bytes.Buffer {
	return sp.Get().(*bytes.Buffer)
}

func BufPut(b *bytes.Buffer) {
	b.Reset()
	sp.Put(b)
}


