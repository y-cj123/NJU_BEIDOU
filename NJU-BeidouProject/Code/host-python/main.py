from PyQt5 import QtCore, QtGui, QtWidgets
#from PyQt5.QtWidgets import QMessageBox,QApplication #弹窗
#from PyQt5.QtGui import QIcon
from AppMainWindow import Ui_MainWindow
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
import sys
import socket
import threading
import time
import ctypes #强制结束线程
import inspect#强制结束线程
import os #文件操作
import binascii


class mywindow(QtWidgets.QMainWindow,Ui_MainWindow,QWidget):
    # 定义一个信号
    signal_write_msg = QtCore.pyqtSignal(str)

    def __init__(self,s_thread):
        super(mywindow, self).__init__()
        self.setupUi(self)
        #设置窗口风格
        #查看支持的风格print(QStyleFactory.keys())
        QApplication.setStyle("Fusion")
        #设置窗体图标
        self.setWindowIcon(QIcon("MianWindow.jpg"))
        #self.setStyleSheet("background-color: rgb(217, 255, 240)")
        #self.pushButton_link.setStyleSheet("background-image: url(MianWindow.jpg)")
        #self.pushButton_unlink.setStyleSheet("background:qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 0, 0, 0), stop:0.52 rgba(0, 0, 0, 0), stop:0.565 rgba(82, 121, 76, 33), stop:0.65 rgba(159, 235, 148, 64), stop:0.721925 rgba(255, 238, 150, 129), stop:0.77 rgba(255, 128, 128, 204), stop:0.89 rgba(191, 128, 255, 64), stop:1 rgba(0, 0, 0, 0))")
        #设置按钮图标
        #self.pushButton_link.setIcon(QIcon("MianWindow.jpg"))
        # 使用socket模块获取本机ip,并显示在相应位置
        #my_address=socket.gethostbyname(socket.gethostname())
        #self.lineEdit_ip.setText(str(my_address))
        addrs = socket.getaddrinfo(socket.gethostname(),None)
        self.idd = "0"
        for item in addrs:
            if ':' not in item[4][0]:
                print(item[4][0])
                self.comboBox_host_ip.insertItem(self.comboBox_host_ip.count(),item[4][0])
        #标志连接状态的标志位以及初始化
        self.link=False
        #连接到本地的客户端列表
        self.client_socket_list=list()
        #按钮状态
        self.pushButton_unlink.setEnabled(True)
        self.pushButton_link.setEnabled(True)

        #信号绑定槽函数，用于显示打印信息
        self.signal_write_msg.connect(self.write_msg)
        #强制结束线程
        self.st=s_thread

        #时间显示
        self.currtime=time.strftime('%Y.%m.%d %H:%M:%S', time.localtime(time.time()))
        self.label_curtime.setText(str(self.currtime))
        #time_th=threading.Thread(target=self.showT())
        #time_th.setDaemon(True)
        #time_th.start()

        # 设置数据层次结构，4行4列
        self.model = QStandardItemModel(0, 3)
        # 设置水平方向四个头标签文本内容
        self.model.setHorizontalHeaderLabels(['地址', '电表编号', '正向有功总(kW·h)'])

        # #Todo 优化2 添加数据
        # self.model.appendRow([
        #     QStandardItem('row %s,column %s' % (11,11)),
        #     QStandardItem('row %s,column %s' % (11,11)),
        #     QStandardItem('row %s,column %s' % (11,11)),
        #     QStandardItem('row %s,column %s' % (11,11)),
        # ])

        #for row in range(4):
        #    for column in range(4):
        #        item = QStandardItem('test')
        #        # 设置每个位置的文本值
        #        self.model.setItem(row, column, item)

        # 实例化表格视图，设置模型为自定义的模型
        self.tableView_history.setModel(self.model)

        # #todo 优化1 表格填满窗口
        # #水平方向标签拓展剩下的窗口部分，填满表格
        self.tableView_history.horizontalHeader().setStretchLastSection(True)
        # #水平方向，表格大小拓展到适当的尺寸
        self.tableView_history.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)

        #item = QStandardItem('app')
        #self.model.setItem(10,3,item)
        #self.tableView_history.setModel(self.model)

    #模板函数，没有用到
    def showT(self):
        while(True):
            currtime = time.strftime('%Y.%m.%d %H:%M:%S', time.localtime(time.time()))
            self.label_curtime.setText(str(currtime))


    #读取电量功能函数
    def readpower(self):
        if self.link is True:
            at1 = self.lineEdit_at1.text()
            send_at1 = binascii.a2b_hex(at1)
            self.cmd_send(send_at1)
            if send_at1 == b'\x0c':
                self.idd = "1"
            if send_at1 == b'\x01\x0c':
                self.idd = "2"
            if send_at1 == b'\x02\x0c':
                self.idd = "3"
            if send_at1 == b'\x03\x0c':
                self.idd = "4"
            if send_at1 == b'\x04\x0c':
                self.idd = "5"
            if send_at1 == b'\x05\x0c':
                self.idd = "6"
            if send_at1 == b'\x06\x0c':
                self.idd = "7"
            if send_at1 == b'\x07\x0c':
                self.idd = "8"
            if send_at1 == b'\x08\x0c':
                self.idd = "9"

    def readpower1(self):
        if self.links is True:
            at2 = self.lineEdit_at2.text()
            send_at2 = binascii.a2b_hex(at2)
            if send_at2 == b'\xa4\x06\x0b\x01\x01\x0c':
                self.idd = "1"
            if send_at2 == b'\xa4\x06\x0b\x01\x01\x01\x0c':
                self.idd = "2"
            if send_at2 == b'\xa4\x06\x0b\x01\x01\x02\x0c':
                self.idd = "3"
            if send_at2 == b'\xa4\x06\x0b\x01\x01\x03\x0c':
                self.idd = "4"
            if send_at2 == b'\xa4\x06\x0b\x01\x01\x04\x0c':
                self.idd = "5"
            if send_at2 == b'\xa4\x06\x0b\x01\x01\x05\x0c':
                self.idd = "6"
            if send_at2 == b'\xa4\x06\x0b\x01\x01\x06\x0c':
                self.idd = "7"
            if send_at2 == b'\xa4\x06\x0b\x01\x01\x07\x0c':
                self.idd = "8"
            if send_at2 == b'\xa4\x06\x0b\x01\x01\x08\x0c':
                self.idd = "9"
            self.tcp_client_socket.send(send_at2)
            self.msg = '已发送给服务器\n'
            self.signal_write_msg.emit('写入')

    def combobox_client_Changed(self):
        print('client_change')

    #连接服务器
    def links(self):
        res=self.client_start()
        if res == 1:
            self.links=True
            self.pushButton_unlink.setEnabled(True)
            self.pushButton_links.setEnabled(False)
            self.msg = '服务器连接成功\n'
            self.signal_write_msg.emit('写入')

    def client_start(self):
        self.tcp_client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.s_port = int(self.lineEdit_port1.text())
            self.s_ip = self.lineEdit_ip.text()
            self.tcp_client_socket.connect((self.s_ip,self.s_port))
        except Exception as ret:
            self.msg='请检查端口号\n'
            self.signal_write_msg.emit('写入')
            QMessageBox.critical(self,"错误","请检查端口号")
            return 0
        else:
            self.client_th=threading.Thread(target=self.tcp_clientThread)
            self.client_th.setDaemon(True)
            self.client_th.start()
            return 1
    def tcp_clientThread(self):
        while True:
            try:
                re_msg=self.tcp_client_socket.recv(1024)
            except Exception as ret:
                pass
            else:
                self.tcp_client_socket.setblocking(False)
                if re_msg:
                    msg=re_msg.hex()
                    self.msg= '已收到来自服务器的数据:\n{}\n'.format(msg)
                    self.signal_write_msg.emit('写入')
                    time.sleep(0.01)
                    self.data_recv(re_msg)


    def link(self):
        """
        单击函数，打开网络连接
        :return:
        """
        #print('link')
        judge=self.tcp_server_start()
        if judge==1:
            self.link=True
            self.pushButton_unlink.setEnabled(True)
            self.pushButton_link.setEnabled(False)
            self.msg = '客户端连接成功\n'
            self.signal_write_msg.emit('写入')

    def tcp_server_start(self):
        """
        功能函数，打开网络连接的方法
        :return:   0:失败 1：成功
        """
        # 创建TCP/UDP套接字
        self.tcp_socket=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        # 将TCP套接字四次挥手后的TIME_WAIT状态取消
        self.tcp_socket.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
        self.tcp_socket.setblocking(False)

        try:
            #addrs = socket.getaddrinfo(socket.gethostname(),None)
            #for item in addrs:
                #if ':' not in item[4][0]:
                    #self.comboBox_host_ip.insertItem(self.comboBox_host_ip.count,item[4][0])
            self.port=int(self.lineEdit_port.text())
            self.ip=self.comboBox_host_ip.currentText()
            self.tcp_socket.bind((self.ip,self.port))

        except Exception as ret:
            self.msg='请检查端口号\n'
            self.signal_write_msg.emit('写入')
            QMessageBox.critical(self,"错误","请检查端口号")
            return 0
        else:
            self.tcp_socket.listen()
            self.server_th=threading.Thread(target=self.tcp_server_concurrency)
            self.server_th.setDaemon(True)
            self.server_th.start()
            self.msg='TCP服务端正在监听端口:%s\n' % str(self.port)
            self.signal_write_msg.emit('写入')
            return 1

    def tcp_server_concurrency(self):
        """
        功能函数，创建TCP服务器线程的方法
        使用非阻塞式并发接收客户端消息
        :return:
        """
        while True:
            self.currtime = time.strftime('%Y.%m.%d %H:%M:%S', time.localtime(time.time()))
            self.label_curtime.setText(str(self.currtime))
            try:
                self.client_socket,self.client_address=self.tcp_socket.accept()
            except Exception as ret:
                time.sleep(0.001)
            else:
                #设置为非阻塞模式
                self.client_socket.setblocking(False)
                #将创建的客户端套接字存入列表并存入界面客户端列表
                self.client_socket_list.append((self.client_socket,self.client_address))
                self.client_showText='{}:{}'.format(self.client_address[0],self.client_address[1])
                self.comboBox_client.insertItem(self.comboBox_client.count(),self.client_showText)
                self.msg= 'TCP服务端已连接IP:%s端口:%s\n' % self.client_address
                self.signal_write_msg.emit('写入')
            #轮询客户端套接字列表，接收数据,单次最多接收1024个字节
            for client,address in self.client_socket_list:
                try:
                    recv_msg=client.recv(1024)
                except Exception as ret:
                    #print(ret)
                    pass
                else:
                    if recv_msg:
                        msg=recv_msg.hex()
                        self.msg= '---->来自IP:{}端口:{}:\n{}\n'.format(address[0], address[1], msg)
                        self.signal_write_msg.emit('写入')
                        time.sleep(0.01)
                        self.data_recv(recv_msg)
                    else:
                        client.close()
                        self.client_socket_list.remove((client,address))
                        self.msg='已断开IP：{}端口：{}的连接'.format(address[0],address[1])
                        self.signal_write_msg.emit('写入')
                        self.client_showText='{}:{}'.format(address[0],address[1])
                        for index in range(self.comboBox_client.count()):
                            if self.comboBox_client.itemText(index)==self.client_showText:
                                self.removeIndex=index
                                break
                        self.comboBox_client.removeItem(self.removeIndex)

    def data_recv(self,data):
        """
        功能函数：接收到的数据进行处理并显示在界面相应位置
        ：param data：byte型数据
        :return:
        """
        try:
            #self.get_address(data)
            #self.get_time(data)
            len=data.__len__()
            #id = self.idd
            if len==4 and data[1]==0xff:
                msg = '系统未准备就绪，请'+str(data[3])+'s后重试\n'
                QMessageBox.about(self, "错误",msg)
            if len>4 and data[6]==0xf9:
                #print("这是一则心跳报文")
                self.msg='来自'+self.get_address(data)+'的心跳报文：'+data+'\n'
                self.signal_write_msg.emit('写入')

            if 20<len<70 and data[6]==0x88:
                #print('data_len=%s' % data.__len__())
                #print("这是一则时间信息报文")
                self.msg = '这是一则时间信息报文\n'
                self.signal_write_msg.emit('写入')
                self.get_time_s(data)

            if 20<len<70 and data[6]==0xb8:
                self.msg = '这是一则电量信息报文\n'
                self.signal_write_msg.emit('写入')
                power=self.get_power(data)
                self.model.appendRow([
                    QStandardItem(power),
                ])
                self.tableView_history.setModel(self.model)

            if len>70 and data[12]==0x0c:
                #print("这是一条电量信息报文")
                self.msg = '这是一则电量信息报文\n'
                self.signal_write_msg.emit('写入')
                time=self.get_time_m(data)
                addr=self.get_address(data)
                on_power=self.get_on_power(data)
                id = self.idd
                #off_power=self.get_off_power(data)
                self.model.appendRow([
                     QStandardItem(addr),
                     QStandardItem(id),
                     QStandardItem(on_power),
                     #QStandardItem(off_power),
                 ])
                self.tableView_history.setModel(self.model)

        except Exception as ret:
            print(ret)
            pass
    #通信测试数据解析
    def get_power(self,data):
        sub_data=data.hex()[39:43]
        power=int(sub_data[3]-0x33)*10000+int(sub_data[2]-0x33)*100+int(sub_data[1]-0x33)*1+int(sub_data[0]-0x33)*0.01
        res = power.strip('0')
        self.lineEdit_on_power.setText(res)
        return res
    def get_address(self,data):
        """
        功能函数：得到集中器地址并显示
        :param data:byte型数据
        :return:
        """
        sub_data=data.hex()[14:22]
        addr = sub_data[2]+sub_data[3]+sub_data[0]+sub_data[1]+sub_data[6]+sub_data[7]+sub_data[4]+sub_data[5]
        self.lineEdit_terminal_add.setText(addr)
        return addr

    def get_time_s(self,data):
        """
        功能函数：从时间信息报文中得到集中器的时间信息并显示,精确到s
        :param data:byte型数据
        :return:
        """
        sub_data=data.hex()[36:48]
        time_month_H=str(int(sub_data[8],16)%2)
        time_data='20'+sub_data[10]+sub_data[11]+'-'+time_month_H+sub_data[9]+'-'+sub_data[6]+sub_data[7]+' '+sub_data[4]+sub_data[5]+':'+sub_data[2]+sub_data[3]+':'+sub_data[0]+sub_data[1]
        self.lineEdit_terminal_time.setText(time_data)
        return time_data

    def get_time_m(self,data):
        """
        功能函数：从电量信息报文中得到集中器的时间信息并显示，精确到分钟
        :param data:byte型数据
        :return:
        """
        sub_data = data.hex()[36:46]
        time_month_H = str(int(sub_data[6], 16) % 2)
        time_data = '20' + sub_data[8] + sub_data[9] + '-' + time_month_H + sub_data[7] + '-' + sub_data[4] + \
                    sub_data[5] + ' ' + sub_data[2] + sub_data[3] + ':' + sub_data[0] + sub_data[1]
        self.lineEdit_terminal_time.setText(time_data)
        return time_data


    def get_on_power(self,data):
        """
        功能函数：获得正向有总并显示
        :param data:byte型数据
        :return:
        """
        sub_data=data.hex()[48:58]
        on_power=sub_data[8]+sub_data[9]+sub_data[6]+sub_data[7]+sub_data[4]+\
                 sub_data[5]+'.'+sub_data[2]+sub_data[3]+sub_data[0]+sub_data[1]
        res = on_power.strip('0')
        self.lineEdit_on_power.setText(res)
        return res

    def get_off_power(self,data):
        """
        功能函数：获得正向无功总并显示
        :param data:byte型数据
        :return:
        此处与学长写的略有不同，待验证
        """
        sub_data=data.hex()[96:104]
        off_power=sub_data[6]+sub_data[7]+sub_data[4]+\
                 sub_data[5]+'.'+sub_data[2]+sub_data[3]+sub_data[0]+sub_data[1]
        res=off_power.strip('0')
        self.lineEdit_off_power.setText(res)
        return res

    def recv_Clear(self):
        #print("recv_clear")
        self.textBrowser_recv.clear()

    def is_hex(self,input_msg):
        for s in input_msg:
            if(not ('0' <= s <= '9' or 'A' <= s <= 'F' or 'a' <= s <= 'f')):
                    return 0
        return 1

    def informationSend(self):
        """
        功能函数，用于TCP服务端和TCP客户端发送消息,点击self.pushButton_send触发的函数
        :return: None
        """
        #print('informationSend')
        if self.checkBox_HEX.isChecked():
            #print("HEX")
            self.msg = '选择HEX编码方式发送\n'
            self.signal_write_msg.emit('写入')
            send_msg = self.textEdit_send.toPlainText()
            send_msg = send_msg.replace(' ', '')
            if(self.is_hex(send_msg)):
                send_msg = bytes.fromhex(send_msg)
                self.cmd_send(send_msg)
            else:
                QMessageBox.critical(self, "错误", "仅支持'03 24 ee AE'类型的hex格式")
        elif self.checkBox_ASCII.isChecked():
            #print("ASCII")
            self.msg = '选择采用ASCII编码方式发送\n'
            self.signal_write_msg.emit('写入')
            send_msg = self.textEdit_send.toPlainText().encode('utf-8')
            self.cmd_send(send_msg)
        else:
            self.msg = '请选择发送编码方式\n'
            self.signal_write_msg.emit('写入')
            QMessageBox.critical(self, "错误", "请选择编码方式")


    def cmd_send(self,send_msg):
        """
        功能函数，向当前客户端发送信息
        :param data:byte型数据
        :return: None
        """
        #print('cmd_send')
        self.msg = '准备向客户端发送数据\n'
        self.signal_write_msg.emit('写入')

        if self.link is False:
            self.msg='请连接网络\n'
            self.signal_write_msg.emit('写入')
            QMessageBox.critical(self, "错误", "请连接网络")
        elif self.comboBox_client.currentText()=='':
            self.msg='请选择客户端\n'
            self.signal_write_msg.emit('写入')
            QMessageBox.critical(self, "错误", "请选择客户端")
        else:
            try:
                #print(send_msg)
                for client,address in self.client_socket_list:
                    current_client='{}:{}'.format(address[0],address[1])
                    if self.comboBox_client.currentText()==current_client:
                        client.send(send_msg)
                        self.msg = '已发送\n'
                        self.signal_write_msg.emit('写入')
                        break
            except Exception as ret:
                #print(ret)
                self.msg='发送失败，错误：'+ret+'\n'
                self.signal_write_msg.emit()

    def informationClear(self):
        #print("informationClear")
        self.textEdit_send.clear()

    def write_msg(self):
        """
        功能函数，向接收区写入数据的方法
        信号-槽触发
        tip：PyQt程序的子线程中，使用非规定的语句向主线程的界面传输字符是不允许的
        :return: None
        """
        self.textBrowser_recv.insertPlainText(self.msg)


    def unlinks(self):
        self.close_client()
        self.links = False
        self.pushButton_unlinks.setEnabled(False)
        self.pushButton_links.setEnabled(True)

    def unlink(self):
        """
        self.pushButton_unlink控件触发
        关闭网络连接
        """
        #print("unlink")
        self.close_all()
        self.link=False
        self.pushButton_unlink.setEnabled(False)
        self.pushButton_link.setEnabled(True)

    def heartbeat(self):
        """
        功能函数：收到心跳报文时的回复报文并显示
        客户端login：{0x68,0x32,0x00,0x32,0x00,0x68,0xD9,0x11,0x20,0x70,0x14,0x00,0x02,0x73,0x00,0x00,0x01,0x00,0x04,0x16}
        :return:68 32 00 32 00 68 00 11   20 70 14 02 00 64 00 00   01 00 1C 16
        ：目前不需要给客户端返回数据
        """
        beat_cmd = b'\x68\x32\x00\x32\x00\x68\x4B\x11\x20\x70\x14\x02\x0C\x60\x00\x00\x02\x00\x70\x16'

    def time_check(self):
        """
        功能函数：查询时间按键触发函数
        :return:
        """
        time_cmd = b'\x68\x32\x00\x32\x00\x68\x4B\x11\x20\x70\x14\x02\x0C\x60\x00\x00\x02\x00\x70\x16'
        self.cmd_send(time_cmd)

    def add_check(self):
        """
        功能函数：查询地址按键触发函数
        :return:
        """
        add_cmd = b'\x68\x32\x00\x32\x00\x68\x4B\x11\x20\x70\x14\x02\x0A\x60\x00\x00\x01\x0B\x78\x16'
        self.cmd_send(add_cmd)

    def close_client(self):
        try:
            self.tcp_client_socket.close()

            if self.links is True:
                self.msg = '已断开服务器\n'
                self.signal_write_msg.emit('写入')
            self.st.stop_thread(self.client_th)
        except Exception as ret:
            pass
    def close_all(self):
        """
        功能函数，关闭网络连接的方法
        :return:
        """
        try:
            for client,address in self.client_socket_list:
                client.close()
                for index in range(self.comboBox_client.count()):
                    self.comboBox_client.removeItem(index)
            self.tcp_socket.close()
            if self.link is True:
                self.msg='已断开客户端\n'
                self.signal_write_msg.emit('写入')
            self.st.stop_thread(self.sever_th)

        except Exception as ret:
            pass

    def closeEvent(self, event):
        """
        重写closeEvent方法，实现dialog窗体关闭时执行一些代码
        :param event: close()触发的事件
        :return: None
        """
        reply = QMessageBox.question(self,
                                     '提示',#标题框名称
                                     "是否要退出系统？",#显示信息
                                     QMessageBox.Yes | QMessageBox.No, #自定义按钮
                                     QMessageBox.Yes) #默认按钮
        if reply == QMessageBox.Yes:
             self.close_all()
             event.accept()
        else:
             event.ignore()

    def history_clear(self):
        self.model.clear()
        self.model.setHorizontalHeaderLabels([ '地址', '编号', '正向有功总（kw.h）'])
        self.tableView_history.setModel(self.model)

    def history_to_txt(self):
        r=self.model.rowCount()
        c=self.model.columnCount()
        currtime = time.strftime('%Y%m%d-%Hh%Mm%Ss', time.localtime(time.time()))
        root_path = os.path.dirname(os.path.realpath(__file__))+'\\'+'HISTORY_DATA'
        if not (os.path.exists(root_path)):
            os.makedirs(root_path)
        filename = root_path + '\\' + currtime + '.txt'
        for i in range(r):
            for j in range(c):
                item=self.model.index(i, j)
                with open(filename, 'a') as file_object:
                    print(item.data(),file=file_object)
        about_message='导出已完成,\n文件路径：'+filename
        QMessageBox.about(self,'提示',about_message)


    def analysis_clear(self):
        self.lineEdit_terminal_add.clear()
        self.lineEdit_terminal_time.clear()
        self.lineEdit_on_power.clear()
        self.lineEdit_off_power.clear()


    def slot1(self):
        print()

class StopThreading:

    @staticmethod  #静态方法无需实例化也可以调用
    def _async_raise(tid, exctype):
        """raises the exception, performs cleanup if needed"""
        tid = ctypes.c_long(tid)
        if not inspect.isclass(exctype):
            exctype = type(exctype)
        res = ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, ctypes.py_object(exctype))
        if res == 0:
            raise ValueError("invalid thread id")
        elif res != 1:
            # """if it returns a number greater than one, you're in trouble,
            # and you should call it again with exc=NULL to revert the effect"""
            ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, None)
            raise SystemError("PyThreadState_SetAsyncExc failed")


    def stop_thread(self,thread):
        self._async_raise(thread.ident, SystemExit)

if __name__ == "__main__":

    app = QtWidgets.QApplication(sys.argv)
    # 设置窗体风格
    #app.setStyle("windowsvista Style")#windowsvista style;Windows style;Fusion style
    st=StopThreading()
    my_ = mywindow(st)
    my_.show()
    sys.exit(app.exec_())