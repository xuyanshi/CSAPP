/*****************************************************************************
 * client-go.go
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

const SEND_BUFFER_SIZE = 2048

/* TODO: client()
 * Open socket and send message from stdin.
 */
func client(server_ip string, server_port string) {
	port, _ := strconv.ParseInt(server_port, 10, 64)
	// 建立服务
	listen, err := net.DialUDP("udp", nil, &net.UDPAddr{
		IP:   net.IP(server_ip),
		Port: int(port),
	})
	if err != nil {
		fmt.Printf("listen udp server error:%v\n", err)
	}
	defer listen.Close()

	// 发送数据
	sendData := []byte("Hello server")
	_, err = listen.Write(sendData) // 发送数据
	if err != nil {
		fmt.Println("发送数据失败，err:", err)
		return
	}
}

// Main parses command-line arguments and calls client function
func main() {
	if len(os.Args) != 3 {
		log.Fatal("Usage: ./client-go [server IP] [server port] < [message file]")
	}
	server_ip := os.Args[1]
	server_port := os.Args[2]
	client(server_ip, server_port)
}
