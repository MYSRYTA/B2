from socket import *
import threading

## UDP受信クラス
class udprecv():
    def __init__(self):

        SrcIP = "127.0.0.1"                             # 受信元IP
        SrcPort = 22222                                 # 受信元ポート番号
        self.SrcAddr = (SrcIP, SrcPort)                 # アドレスをtupleに格納

        self.BUFSIZE = 1024                             # バッファサイズ指定
        self.udpServSock = socket(AF_INET, SOCK_DGRAM)  # ソケット作成
        self.udpServSock.bind(self.SrcAddr)             # 受信元アドレスでバインド

    def recv(self):
        while True:                                     # 常に受信待ち

            data, addr = self.udpServSock.recvfrom(self.BUFSIZE)
                                                        # 受信
            print(data.decode(), addr)                  # 受信データと送信アドレス表示
            
            if data.decode() == "AAAAAAAAA":
                print("--------END----------")
                break
        self.udpServSock.close()

udp = udprecv()     # クラス呼び出し
udp_thread = threading.Thread(target=udp.recv)
udp_thread.daemon = True  # メインスレッドが終了した際に、このスレッドも終了するように設定
udp_thread.start()
