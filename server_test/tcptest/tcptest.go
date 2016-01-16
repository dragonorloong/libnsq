package main

import 
(
"fmt"
"net"
"time"
"log"
)

//连接服务器
func connectServer() {
    //接通
    conn, err := net.Dial("tcp", "10.10.200.130:12198")
	if err != nil {
		log.Fatal("an error!", err.Error())
		return;
	}
	
    for {
            buf := make([]byte, 512);
            conn.Write([]byte("GET /test?test HTTP/1.0\r\nHost: 10.10.200.130:12198\r\nUser-Agent: ApacheBench/2.3\r\nAccept: */*\r\n\r\n"));
            conn.Read(buf);
//            fmt.Println(string(buf));
            //sleep(1);
            time.Sleep(10*time.Second);
    }
}

//主函数
func main() {
	//连接servser
    for a := 0; a < 60000; a++ {
	    go connectServer()

        if (a % 10 == 0) {
            //sleep(1);
            time.Sleep(3*time.Millisecond);
        }

        fmt.Printf("a = %d\n", a);
    }

	time.Sleep(10000000*time.Second);
}
