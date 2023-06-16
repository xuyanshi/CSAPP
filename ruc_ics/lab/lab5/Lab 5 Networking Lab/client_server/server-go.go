/*****************************************************************************
 * server-go.go
 * Name:
 * NetId:
 *****************************************************************************/

package main

import (
	"fmt"
	"log"
	"net"
	"os"
	"strconv"
)

const RECV_BUFFER_SIZE = 2048

/* TODO: server()
 * Open socket and wait for client to connect
 * Print received message to stdout
 */
func server(server_port string) {
	port, _ := strconv.ParseInt(server_port, 10, 64)
	listen, err := net.ListenUDP("udp", &net.UDPAddr{
		IP:   net.IPv4(0, 0, 0, 0),
		Port: int(port),
	})
	if err != nil {
		fmt.Printf("listen failed error:%v\n", err)
		return
	}
	defer listen.Close() // 使用完关闭服务

	for {
		// 接收数据
		var data [RECV_BUFFER_SIZE]byte
		n, addr, err := listen.ReadFromUDP(data[:])
		if err != nil {
			fmt.Printf("read data error:%v\n", err)
			return
		}
		fmt.Printf("addr:%v\t count:%v\t data:%v\n", addr, n, string(data[:n]))
		// 发送数据
		_, err = listen.WriteToUDP(data[:n], addr)
		if err != nil {
			fmt.Printf("send data error:%v\n", err)
			return
		}
	}
}

// Main parses command-line arguments and calls server function
func main() {
	if len(os.Args) != 2 {
		log.Fatal("Usage: ./server-go [server port]")
	}
	server_port := os.Args[1]
	server(server_port)
}
