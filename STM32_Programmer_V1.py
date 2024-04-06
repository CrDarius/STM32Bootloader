import serial
import struct
import os
import sys
import glob

Flash_HAL_OK                                        = 0x00
Flash_HAL_ERROR                                     = 0x01
Flash_HAL_BUSY                                      = 0x02
Flash_HAL_TIMEOUT                                   = 0x03
Flash_HAL_INV_ADDR                                  = 0x04

#BL Commands

COMMAND_BL_GET_VER                                  = 0x10
COMMAND_BL_GET_CID                                  = 0x20
COMMAND_BL_FLASH_ERASE                              = 0x30
COMMAND_BL_MEM_WRITE                                = 0x40
COMMAND_BL_MEM_VERIFY                               = 0x50
COMMAND_BL_GET_RDP_STATUS                           = 0x60
COMMAND_BL_EN_R_W_PROTECT                           = 0x70
COMMAND_BL_JMP_TO_USERAPP                           = 0x80
COMMAND_BL_DIS_R_W_PROTECT                          = 0x90

COMMAND_BL_READ_SECTOR_P_STATUS                     =0x56


#len details of the command
NUMBER_OF_CONTROL_BYTES                     = 7 # 1 byte  - Frame Type
                                                # 1 byte  - CMD
                                                # 1 byte  - Data Length
                                                # 4 bytes - CRC
NUMBER_OF_HEADER_BYTES                      = 3

COMMAND_BL_GET_VER_DATALEN                  = 0
COMMAND_BL_GET_CID_DATALEN                  = 0
COMMAND_BL_GET_RDP_STATUS_DATALEN           = 0
COMMAND_BL_FLASH_ERASE_DATALEN              = 1            
COMMAND_BL_MEM_WRITE_DATALEN                = 0            
COMMAND_BL_EN_R_W_PROTECT_DATALEN           = 0            
COMMAND_BL_READ_SECTOR_P_STATUS_DATALEN     = 0
COMMAND_BL_JMP_TO_USERAPP_DATALEN           = 0            
COMMAND_BL_DIS_R_W_PROTECT_DATALEN          = 0             

verbose_mode = 1
mem_write_active =0

#----------------------------- file ops----------------------------------------

def calc_file_len():
    size = os.path.getsize("user_app.bin")
    return size

def open_the_file():
    global bin_file
    bin_file = open('user_app.bin','rb')
    #read = bin_file.read()
    #global file_contents = bytearray(read)

def read_the_file():
    pass

def close_the_file():
    bin_file.close()




#----------------------------- utilities----------------------------------------

def word_to_byte(addr, index , lowerfirst):
    value = (addr >> ( 8 * ( index -1)) & 0x000000FF )
    return value

def get_crc(buff, length):
    Crc = 0xFFFFFFFF
    #print(length)
    for data in buff[0:length]:
        Crc = Crc ^ data
        for i in range(32):
            if(Crc & 0x80000000):
                Crc = (Crc << 1) ^ 0x04C11DB7
            else:
                Crc = (Crc << 1)
    return Crc

#----------------------------- Serial Port ----------------------------------------
def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

def Serial_Port_Configuration(port):
    global ser
    try:
        ser = serial.Serial(port, baudrate = 9600, bytesize = 8, parity = 'N', stopbits = 2, timeout = 10)
    except:
        print("\n   Oops! That was not a valid port")
        
        port = serial_ports()
        if(not port):
            print("\n   No ports Detected")
        else:
            print("\n   Here are some available ports on your PC. Try Again!")
            print("\n   ",port)
        return -1
    if ser.is_open:
        print("\n   Port Open Success")
    else:
        print("\n   Port Open Failed")
    return 0

              
def read_serial_port(length):
    read_value = ser.read(length)
    return read_value

def Close_serial_port():
    pass
def purge_serial_port():
    ser.reset_input_buffer()
    
def Write_to_serial_port(value, *length):
        data = struct.pack('>B', value)
        if (verbose_mode):
            value = bytearray(data)
            #print("   "+hex(value[0]), end='')
            print("   "+"0x{:02x}".format(value[0]),end=' ')
        if(mem_write_active and (not verbose_mode)):
                print("#",end=' ')
        ser.write(data)


        
#----------------------------- command processing----------------------------------------

def process_COMMAND_BL_GET_VER(length):
    ver=read_serial_port(length)
    ascii_ver = ''.join([chr(byte) for byte in ver])
    print("\n   Bootloader Ver. : ", ascii_ver)

def process_COMMAND_BL_GET_CID(length):
    value = read_serial_port(length)
    device_id = (value[1] << 8) + value[0]
    revision_id = (value[3] << 8) + value[2]
    print("\n   Device ID    : ", hex(device_id))
    print("\n   Revision ID  : ", hex(revision_id))


def process_COMMAND_BL_GET_RDP_STATUS(length):
    value = read_serial_port(length)
    rdp = bytearray(value)
    print("\n   RDP Status : ",hex(rdp[0]))

def process_COMMAND_BL_FLASH_ERASE(length):
    value = read_serial_port(length)
    erase_status = bytearray(value)
    if len(value):
        for i in range(len(erase_status)):
            if(erase_status[i] == 0xAA):
                print("\n Erase Status of Sector", i, ": Success")
            elif(erase_status[i] == 0xFF):
                print("\n Erase Status of Sector", i, ": Failure")
            else:
                print("\n Erase Status of Sector", i, ": Not requested")  
    else:
        print("Timeout: Bootloader is not responding")

def process_COMMAND_BL_MEM_WRITE(length):
    write_status=0
    value = read_serial_port(length)
    write_status = bytearray(value)
    if(write_status[0] == Flash_HAL_OK):
        print("\n   Write_status: FLASH_HAL_OK")
    elif(write_status[0] == Flash_HAL_ERROR):
        print("\n   Write_status: FLASH_HAL_ERROR")
    elif(write_status[0] == Flash_HAL_BUSY):
        print("\n   Write_status: FLASH_HAL_BUSY")
    elif(write_status[0] == Flash_HAL_TIMEOUT):
        print("\n   Write_status: FLASH_HAL_TIMEOUT")
    elif(write_status[0] == Flash_HAL_INV_ADDR):
        print("\n   Write_status: FLASH_HAL_INV_ADDR")
    else:
        print("\n   Write_status: UNKNOWN_ERROR")
    print("\n")

protection_mode= [ "Write Protection", "Read/Write Protection","No protection" ]
def protection_type(status,n):
    if( status & (1 << 15) ):
        #PCROP is active
        if(status & (1 << n) ):
            return protection_mode[1]
        else:
            return protection_mode[2]
    else:
        if(status & (1 << n)):
            return protection_mode[2]
        else:
            return protection_mode[0]
            
        
        
        
def process_COMMAND_BL_READ_SECTOR_STATUS(length):
    s_status=0

    value = read_serial_port(length)
    s_status = bytearray(value)
    #s_status.flash_sector_status = (uint16_t)(status[1] << 8 | status[0] )
    print("\n   Sector Status : ",s_status[0])
    print("\n  ====================================")
    print("\n  Sector                               \tProtection") 
    print("\n  ====================================")
    if(s_status[0] & (1 << 15)):
        #PCROP is active
        print("\n  Flash protection mode : Read/Write Protection(PCROP)\n")
    else:
        print("\n  Flash protection mode :   \tWrite Protection\n")

    for x in range(8):
        print("\n   Sector{0}                               {1}".format(x,protection_type(s_status[0],x) ) )
        


def process_COMMAND_BL_DIS_R_W_PROTECT(length):
    status=0
    value = read_serial_port(length)
    status = bytearray(value)
    if(status[0]):
        print("\n   FAIL")
    else:
        print("\n   SUCCESS")

def process_COMMAND_BL_EN_R_W_PROTECT(length):
    status=0
    value = read_serial_port(length)
    status = bytearray(value)
    if(status[0]):
        print("\n   FAIL")
    else:
        print("\n   SUCCESS")

def process_COMMAND_BL_JMP_TO_USERAPP(length):
    print("\n   ###Jumping to user application###")


def decode_menu_command_code(command):
    ret_value = 0
    data_buf = []
    for i in range(255):
        data_buf.append(0)
    
    if(command  == 0 ):
        print("\n   Exiting...!")
        raise SystemExit
    elif(command == 1):
        print("\n   Command == > BL_GET_VER")
        FRAME_TYPE  = 0 
        data_buf[0] = FRAME_TYPE                 
        data_buf[1] = COMMAND_BL_GET_VER
        data_buf[2] = COMMAND_BL_GET_VER_DATALEN 
        crc32       = get_crc(data_buf, COMMAND_BL_GET_VER_DATALEN + NUMBER_OF_HEADER_BYTES)
        crc32 = crc32 & 0xffffffff
        data_buf[3] = word_to_byte(crc32,1,1) 
        data_buf[4] = word_to_byte(crc32,2,1) 
        data_buf[5] = word_to_byte(crc32,3,1) 
        data_buf[6] = word_to_byte(crc32,4,1) 

        
        Write_to_serial_port(data_buf[0],1)
        for i in data_buf[1:COMMAND_BL_GET_VER_DATALEN + NUMBER_OF_CONTROL_BYTES]:
            Write_to_serial_port(i, COMMAND_BL_GET_VER_DATALEN + NUMBER_OF_CONTROL_BYTES - 1)
        
        ret_value = read_bootloader_reply(data_buf[1])
        
    elif(command == 2):
        print("\n   Command == > BL_GET_CID")
        FRAME_TYPE = 0
        data_buf[0] = FRAME_TYPE
        data_buf[1] = COMMAND_BL_GET_CID
        data_buf[2] =  COMMAND_BL_GET_CID_DATALEN
        crc32       = get_crc(data_buf, COMMAND_BL_GET_CID_DATALEN + NUMBER_OF_HEADER_BYTES)
        crc32 = crc32 & 0xffffffff
        data_buf[3] = word_to_byte(crc32,1,1) 
        data_buf[4] = word_to_byte(crc32,2,1) 
        data_buf[5] = word_to_byte(crc32,3,1) 
        data_buf[6] = word_to_byte(crc32,4,1) 

        
        Write_to_serial_port(data_buf[0],1)
        for i in data_buf[1:COMMAND_BL_GET_CID_DATALEN + NUMBER_OF_CONTROL_BYTES]:
            Write_to_serial_port(i, COMMAND_BL_GET_CID_DATALEN + NUMBER_OF_CONTROL_BYTES - 1)
        

        ret_value = read_bootloader_reply(data_buf[1])

    elif(command == 3):
        print("\n   Command == > BL_GET_RDP_STATUS")
        FRAME_TYPE = 0
        data_buf[0] = FRAME_TYPE
        data_buf[1] = COMMAND_BL_GET_RDP_STATUS
        data_buf[2] = COMMAND_BL_GET_RDP_STATUS_DATALEN
        crc32       = get_crc(data_buf,COMMAND_BL_GET_RDP_STATUS_DATALEN + NUMBER_OF_HEADER_BYTES)
        crc32 = crc32 & 0xffffffff
        data_buf[3] = word_to_byte(crc32,1,1)
        data_buf[4] = word_to_byte(crc32,2,1)
        data_buf[5] = word_to_byte(crc32,3,1)
        data_buf[6] = word_to_byte(crc32,4,1)
        
        Write_to_serial_port(data_buf[0],1)
        
        for i in data_buf[1:COMMAND_BL_GET_RDP_STATUS_DATALEN + NUMBER_OF_CONTROL_BYTES]:
            Write_to_serial_port(i,COMMAND_BL_GET_RDP_STATUS_DATALEN + NUMBER_OF_CONTROL_BYTES -1)
        
        ret_value = read_bootloader_reply(data_buf[1])
    
    elif(command == 4):
        print("\n   Command == > BL_FLASH_ERASE")
        FRAME_TYPE  = 0
        data_buf[0] = FRAME_TYPE
        data_buf[1] = COMMAND_BL_FLASH_ERASE
        data_buf[2] = COMMAND_BL_FLASH_ERASE_DATALEN
        sector_num = input("\n   Enter the sectors to be erased as a one byte bit mask (Ex: 11111100 -> erase [Sector 2, Sector 7]): ")
        sector_num = int(sector_num, 2)
        data_buf[3]= sector_num 

        crc32       = get_crc(data_buf,COMMAND_BL_FLASH_ERASE_DATALEN + NUMBER_OF_HEADER_BYTES) 
        data_buf[4] = word_to_byte(crc32,1,1) 
        data_buf[5] = word_to_byte(crc32,2,1) 
        data_buf[6] = word_to_byte(crc32,3,1) 
        data_buf[7] = word_to_byte(crc32,4,1) 

        Write_to_serial_port(data_buf[0],1)
        
        for i in data_buf[1:COMMAND_BL_FLASH_ERASE_DATALEN + NUMBER_OF_CONTROL_BYTES]:
            Write_to_serial_port(i, COMMAND_BL_FLASH_ERASE_DATALEN + NUMBER_OF_CONTROL_BYTES - 1)
        
        ret_value = read_bootloader_reply(data_buf[1])
        
    elif(command == 5):
        print("\n   Command == > BL_MEM_WRITE")
        bytes_remaining=0
        t_len_of_file=0
        bytes_so_far_sent = 0
        len_to_read=0
        base_mem_address=0

        data_buf[1] = COMMAND_BL_MEM_WRITE

        #First get the total number of bytes in the .bin file.
        t_len_of_file =calc_file_len()

        #keep opening the file
        open_the_file()

        bytes_remaining = t_len_of_file - bytes_so_far_sent

        base_mem_address = input("\n   Enter the memory write address here :")
        base_mem_address = int(base_mem_address, 16)
        global mem_write_active
        while(bytes_remaining):
            mem_write_active=1
            if(bytes_remaining >= 128):
                len_to_read = 128
            else:
                len_to_read = bytes_remaining
            #get the bytes in to buffer by reading file
            for x in range(len_to_read):
                file_read_value = bin_file.read(1)
                file_read_value = bytearray(file_read_value)
                data_buf[7+x]= int(file_read_value[0])
            #read_the_file(&data_buf[7],len_to_read) 
            #print("\n   base mem address = \n",base_mem_address, hex(base_mem_address)) 

            #populate base mem address
            data_buf[2] = word_to_byte(base_mem_address,1,1)
            data_buf[3] = word_to_byte(base_mem_address,2,1)
            data_buf[4] = word_to_byte(base_mem_address,3,1)
            data_buf[5] = word_to_byte(base_mem_address,4,1)

            data_buf[6] = len_to_read

            #/* 1 byte len + 1 byte command code + 4 byte mem base address
            #* 1 byte payload len + len_to_read is amount of bytes read from file + 4 byte CRC
            #*/
            mem_write_cmd_total_len = COMMAND_BL_MEM_WRITE_DATALEN+len_to_read

            #first field is "len_to_follow"
            data_buf[0] =mem_write_cmd_total_len-1

            crc32       = get_crc(data_buf,mem_write_cmd_total_len-4)
            data_buf[7+len_to_read] = word_to_byte(crc32,1,1)
            data_buf[8+len_to_read] = word_to_byte(crc32,2,1)
            data_buf[9+len_to_read] = word_to_byte(crc32,3,1)
            data_buf[10+len_to_read] = word_to_byte(crc32,4,1)

            #update base mem address for the next loop
            base_mem_address+=len_to_read

            Write_to_serial_port(data_buf[0],1)
        
            for i in data_buf[1:mem_write_cmd_total_len]:
                Write_to_serial_port(i,mem_write_cmd_total_len-1)

            bytes_so_far_sent+=len_to_read
            bytes_remaining = t_len_of_file - bytes_so_far_sent
            print("\n   bytes_so_far_sent:{0} -- bytes_remaining:{1}\n".format(bytes_so_far_sent,bytes_remaining)) 
        
            ret_value = read_bootloader_reply(data_buf[1])
        mem_write_active=0

            
    
    elif(command == 6):
        print("\n   Command == > BL_EN_R_W_PROTECT")
        total_sector = int(input("\n   How many sectors do you want to protect ?: "))
        sector_numbers = [0,0,0,0,0,0,0,0]
        sector_details=0
        for x in range(total_sector):
            sector_numbers[x]=int(input("\n   Enter sector number[{0}]: ".format(x+1) ))
            sector_details = sector_details | (1 << sector_numbers[x])

        #print("Sector details",sector_details)
        print("\n   Mode:Flash sectors Write Protection: 1")
        print("\n   Mode:Flash sectors Read/Write Protection: 2")
        mode=input("\n   Enter Sector Protection Mode(1 or 2 ):")
        mode = int(mode)
        if(mode != 2 and mode != 1):
            print("\n   Invalid option : Command Dropped")
            return
        if(mode == 2):
            print("\n   This feature is currently not supported !") 
            return

        data_buf[0] = COMMAND_BL_EN_R_W_PROTECT_DATALEN-1 
        data_buf[1] = COMMAND_BL_EN_R_W_PROTECT 
        data_buf[2] = sector_details 
        data_buf[3] = mode 
        crc32       = get_crc(data_buf,COMMAND_BL_EN_R_W_PROTECT_DATALEN-4) 
        data_buf[4] = word_to_byte(crc32,1,1) 
        data_buf[5] = word_to_byte(crc32,2,1) 
        data_buf[6] = word_to_byte(crc32,3,1) 
        data_buf[7] = word_to_byte(crc32,4,1) 

        Write_to_serial_port(data_buf[0],1)
        
        for i in data_buf[1:COMMAND_BL_EN_R_W_PROTECT_DATALEN]:
            Write_to_serial_port(i,COMMAND_BL_EN_R_W_PROTECT_DATALEN-1)
        
        ret_value = read_bootloader_reply(data_buf[1])
            
    elif(command == 7):
        print("\n   Command == > COMMAND_BL_READ_SECTOR_P_STATUS")
        FRAME_TYPE  = 0
        data_buf[0] = FRAME_TYPE
        data_buf[1] = COMMAND_BL_READ_SECTOR_P_STATUS
        data_buf[2] = COMMAND_BL_READ_SECTOR_P_STATUS_DATALEN
        crc32       = get_crc(data_buf,COMMAND_BL_READ_SECTOR_P_STATUS_DATALEN + NUMBER_OF_HEADER_BYTES) 
        data_buf[3] = word_to_byte(crc32,1,1) 
        data_buf[4] = word_to_byte(crc32,2,1) 
        data_buf[5] = word_to_byte(crc32,3,1) 
        data_buf[6] = word_to_byte(crc32,4,1) 

        Write_to_serial_port(data_buf[0],1)
        
        for i in data_buf[1:COMMAND_BL_READ_SECTOR_P_STATUS_DATALEN + NUMBER_OF_CONTROL_BYTES]:
            Write_to_serial_port(i,COMMAND_BL_READ_SECTOR_P_STATUS_DATALEN + NUMBER_OF_CONTROL_BYTES -1)
        
        ret_value = read_bootloader_reply(data_buf[1])

    elif(command == 8):
        print("\n   Command == > COMMAND_BL_JMP_TO_USERAPP")
        FRAME_TYPE  = 0
        data_buf[0] = FRAME_TYPE
        data_buf[1] = COMMAND_BL_JMP_TO_USERAPP
        data_buf[2] = COMMAND_BL_JMP_TO_USERAPP_DATALEN
        crc32       = get_crc(data_buf,COMMAND_BL_JMP_TO_USERAPP_DATALEN + NUMBER_OF_HEADER_BYTES) 
        data_buf[3] = word_to_byte(crc32,1,1) 
        data_buf[4] = word_to_byte(crc32,2,1) 
        data_buf[5] = word_to_byte(crc32,3,1) 
        data_buf[6] = word_to_byte(crc32,4,1) 

        Write_to_serial_port(data_buf[0],1)
        
        for i in data_buf[1:COMMAND_BL_JMP_TO_USERAPP_DATALEN + NUMBER_OF_CONTROL_BYTES]:
            Write_to_serial_port(i,COMMAND_BL_JMP_TO_USERAPP_DATALEN + NUMBER_OF_CONTROL_BYTES -1)
        
        ret_value = read_bootloader_reply(data_buf[1])

    elif(command == 9):
        print("\n   Command == > COMMAND_BL_DIS_R_W_PROTECT")
        data_buf[0] = COMMAND_BL_DIS_R_W_PROTECT_DATALEN-1 
        data_buf[1] = COMMAND_BL_DIS_R_W_PROTECT 
        crc32       = get_crc(data_buf,COMMAND_BL_DIS_R_W_PROTECT_DATALEN-4) 
        data_buf[2] = word_to_byte(crc32,1,1) 
        data_buf[3] = word_to_byte(crc32,2,1) 
        data_buf[4] = word_to_byte(crc32,3,1) 
        data_buf[5] = word_to_byte(crc32,4,1) 

        Write_to_serial_port(data_buf[0],1)
        
        for i in data_buf[1:COMMAND_BL_DIS_R_W_PROTECT_DATALEN]:
            Write_to_serial_port(i,COMMAND_BL_DIS_R_W_PROTECT_DATALEN-1)
        
        ret_value = read_bootloader_reply(data_buf[1])
        
    else:
        print("\n   Please input valid command code\n")
        return

    if ret_value == -2 :
        print("\n   TimeOut : No response from the bootloader")
        print("\n   Reset the board and Try Again !")
        return

def read_bootloader_reply(command_code):
    #ack=[0,0]
    len_to_follow=0 
    ret = -2 

    #read_serial_port(ack,2)
    #ack = ser.read(2)
    ack=read_serial_port(3)
    if(len(ack)):
        a_array=bytearray(ack)
        #print("read uart:",ack) 
        if (a_array[1]== 0xAA):
            #CRC of last command was good .. received ACK and "len to follow"
            len_to_follow=a_array[2]
            print("\n   CRC : SUCCESS Len :",len_to_follow)
            #print("command_code:",hex(command_code))
            if (command_code) == COMMAND_BL_GET_VER :
                process_COMMAND_BL_GET_VER(len_to_follow)
                                
            elif(command_code) == COMMAND_BL_GET_CID:
                process_COMMAND_BL_GET_CID(len_to_follow)
                
            elif(command_code) == COMMAND_BL_GET_RDP_STATUS:
                process_COMMAND_BL_GET_RDP_STATUS(len_to_follow)
                    
            elif(command_code) == COMMAND_BL_FLASH_ERASE:
                process_COMMAND_BL_FLASH_ERASE(len_to_follow)
                
            elif(command_code) == COMMAND_BL_MEM_WRITE:
                process_COMMAND_BL_MEM_WRITE(len_to_follow)
                
            elif(command_code) == COMMAND_BL_READ_SECTOR_P_STATUS:
                process_COMMAND_BL_READ_SECTOR_STATUS(len_to_follow)
                
            elif(command_code) == COMMAND_BL_EN_R_W_PROTECT:
                process_COMMAND_BL_EN_R_W_PROTECT(len_to_follow)

            elif(command_code) == COMMAND_BL_JMP_TO_USERAPP:
                process_COMMAND_BL_JMP_TO_USERAPP(len_to_follow)
                
            elif(command_code) == COMMAND_BL_DIS_R_W_PROTECT:
                process_COMMAND_BL_DIS_R_W_PROTECT(len_to_follow)
                
            else:
                print("\n   Invalid command code\n")
                
            ret = 0
         
        elif a_array[1] == 0xFF:
            #CRC of last command was bad .. received NACK
            print("\n   CRC: FAIL \n")
            ret= -1
    else:
        print("\n   Timeout : Bootloader not responding")
        
    return ret

            
            

#----------------------------- Ask Menu implementation----------------------------------------


name = input("Enter the Port Name of your device(Ex: COM3):")
ret = 0
ret=Serial_Port_Configuration(name)
if(ret < 0):
    decode_menu_command_code(0)
    

    
  
while True:
    print("\n +==========================================+")
    print(" |               Menu                       |")
    print(" |         STM32F4 BootLoader v1            |")
    print(" +==========================================+")
    print("\n   Which BL command do you want to send ?")
    print("   BL_GET_VER                            --> 1")
    print("   BL_GET_CID                            --> 2")
    print("   BL_GET_RDP_STATUS                     --> 3")
    print("   BL_FLASH_ERASE                        --> 4")
    print("   BL_MEM_WRITE                          --> 5")
    print("   BL_EN_R_W_PROTECT                     --> 6")
    print("   BL_READ_SECTOR_P_STATUS               --> 7")
    print("   BL_JUMP_TO_USER_APP                   --> 8")
    print("   BL_DIS_R_W_PROTECT                    --> 9")
    print("   MENU_EXIT                             --> 0")

    #command_code = int(input("\n   Type the command code here :") )

    command_code = input("\n   Type the command code here :")

    if(not command_code.isdigit()):
        print("\n   Please Input valid code shown above")
    else:
        decode_menu_command_code(int(command_code))

    input("\n   Press any key to continue  :")
    purge_serial_port()





    

def check_flash_status():
    pass

def protection_type():
    pass

