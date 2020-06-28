import struct

if __name__ == "__main__":
    msg='68 74 00 EE 74 70 00 00'
    mk=b'68 74'
    print(mk.decode('utf-8'))
    #去掉空格
    msg=msg.replace(' ','')

    #字面转换为字节型数据
    a_bytes=bytes.fromhex(msg)
    #数据长度
    print(a_bytes.__len__())
    print(a_bytes)

    #bytes.formhex(string)
    #string 必须是2个字符的16进制的形式，'1662 6a 6b' ,空格将被忽略！
    #返回16进制表示的字符串
    data=a_bytes.hex()
    print(data)
    #索引：返回该字节对应的字面字符
    print(int(data[6],16))

    #解码，字节型转换为ASSCII表示的字符串
#    print(a_bytes.decode('utf-8'))
    print(data)

    #按字面转换为字节型
    data=msg.encode('utf-8')
    print(data)
