// main.go
package main

import (
	"fmt"
	"io/ioutil"

	"./decode"
)

func main() {
	b, err := ioutil.ReadFile("/Users/bingjian/Desktop/3536491.xlog") // just pass the file name
	if err != nil {
		fmt.Print(err)
	}

	buf, err := decode.Decode(b)
	if err != nil {
		fmt.Println("err", err)
		return
	}
	fmt.Println("buf", string(buf))
}
