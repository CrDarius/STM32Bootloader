import serial
import struct
import os
import sys
import glob

#FLASH Protection Levels
W_PROTECTION                = 0x0F
RW_PROTECTION               = 0x1F
NO_PROTECTION               = 0xFF

#BL Commands

COMMAND_BL_GET_VER                                  = 0x10
COMMAND_BL_GET_CID                                  = 0x20
COMMAND_BL_FLASH_ERASE                              = 0x30
COMMAND_BL_MEM_WRITE                                = 0x40
COMMAND_BL_MEM_VERIFY                               = 0x50
COMMAND_BL_GET_RDP_STATUS                           = 0x60
COMMAND_BL_GET_RW_SECTOR_PROT                       = 0x70
COMMAND_BL_JMP_TO_USERAPP                           = 0x80
COMMAND_BL_SET_RW_SECTOR_PROT                       = 0x90



#len details of the command
NUMBER_OF_COMMAND_BYTES                     = 1
NUMBER_OF_FRAME_TYPE_BYTES                  = 1
NUMBER_OF_DATALEN_BYTES                     = 1
NUMBER_OF_CRC_BYTES                         = 4
NUMBER_OF_HEADER_BYTES                      = NUMBER_OF_FRAME_TYPE_BYTES + NUMBER_OF_COMMAND_BYTES + NUMBER_OF_DATALEN_BYTES
NUMBER_OF_CONTROL_BYTES                     = NUMBER_OF_HEADER_BYTES + NUMBER_OF_CRC_BYTES

PROG_BLOCK_MAX_SIZE                         = 255

COMMAND_BL_GET_VER_DATALEN                  = 0
COMMAND_BL_GET_CID_DATALEN                  = 0
COMMAND_BL_GET_RDP_STATUS_DATALEN           = 0
COMMAND_BL_FLASH_ERASE_DATALEN              = 1
COMMAND_BL_MEM_WRITE_DATALEN                = 0
COMMAND_BL_GET_RW_SECTOR_PROT_DATALEN       = 0
COMMAND_BL_SET_RW_SECTOR_PROT_DATALEN       = 0
COMMAND_BL_JMP_TO_USERAPP_DATALEN           = 0


LAST_FRAME                                  = 0
CONSECUTIVE_FRAME                           = 1

POS_FRAME_TYPE                              = 0
POS_COMMAND                                 = 1
POS_DATALEN                                 = 2
POS_STATUS                                  = 1

NUMBER_OF_TARGET_FLASH_SECTORS              = 8

verbose_mode = 1
mem_write_active =0

#----------------------------- file ops----------------------------------------

def calc_file_len(file_path):
    size = os.path.getsize(file_path)
    return size

def open_the_file(file_path):
    global bin_file
    bin_file = open(file_path,'rb')

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
    
def Write_to_serial_port(data_buf, len):
        for value in data_buf[0 : len]:
            data = struct.pack('>B', value)
            if (verbose_mode):
                value = bytearray(data)
                print("   "+"0x{:02x}".format(value[0]),end=' ')
            if((mem_write_active == 2) and (not verbose_mode)):
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
    
    value = read_serial_port(length)
    value_int = int.from_bytes(value, byteorder='little')
    
    #The Bootloader will erase the necessary sectors before programming them
    if(mem_write_active == 0):
        print("\n   The following sectors will be erased: ")
        for i in range(NUMBER_OF_TARGET_FLASH_SECTORS):
            if((value_int >> i) & 1):
                print("\n Sector: ", i)
    elif(mem_write_active == 1):
        print("\n###########################################")
        for i in range(NUMBER_OF_TARGET_FLASH_SECTORS):
            if((value_int >> i) & 1):
                print("\n SECTOR: ", i, "ERASED")
    else:
        print("\n Block successfully programmed")
        
        
    
    
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
    print("\n   Sector Status : ",s_status[0])
    print("\n  ====================================")
    print("\n  Sector                               \tProtection") 
    print("\n  ====================================")
    if(s_status[0] & (1 << 15)):
        #PCROP is active
        print("\n  Flash protection mode : Read/Write Protection(PCROP)\n")
    else:
        print("\n  Flash protection mode :   \tWrite Protection\n")

    for x in range(NUMBER_OF_TARGET_FLASH_SECTORS):
        print("\n   Sector{0}                               {1}".format(x,protection_type(s_status[0],x) ) )
        

def process_COMMAND_BL_GET_RW_SECTOR_PROT(length):
    value = read_serial_port(length)
    prot = bytearray(value)
    for i in range(NUMBER_OF_TARGET_FLASH_SECTORS):
        if(prot[i] == W_PROTECTION):
            print(f"\n Sector {i}: WriteProtection")
        elif(prot[i] == RW_PROTECTION):
            print(f"\n Sector {i}: WriteProtection")
        elif(prot[i] == NO_PROTECTION):
            print(f"\n Sector {i}: NoProtection")
        else:
            print(f"\n Sector {i}: ERROR")

def process_COMMAND_BL_JMP_TO_USERAPP(length):
    print("\n   ###Jumping to user application###")


def decode_menu_command_code(command):
    global mem_write_active
    ret_value = 0
    data_buf = []
    for i in range(512):
        data_buf.append(0)
    
    if(command  == 0 ):
        print("\n   Exiting...!")
        raise SystemExit
    
    elif(command == 1):
        print("\n   Command == > BL_GET_VER")
        FRAME_TYPE  = LAST_FRAME
        data_buf[POS_FRAME_TYPE] = FRAME_TYPE                 
        data_buf[POS_COMMAND] = COMMAND_BL_GET_VER
        data_buf[POS_DATALEN] = COMMAND_BL_GET_VER_DATALEN 

        apply_4byte_crc(data_buf, COMMAND_BL_GET_VER_DATALEN + NUMBER_OF_HEADER_BYTES)

        Write_to_serial_port(data_buf, COMMAND_BL_GET_VER_DATALEN + NUMBER_OF_CONTROL_BYTES)
        
        ret_value = read_bootloader_reply(data_buf[1])
        
    elif(command == 2):
        print("\n   Command == > BL_GET_CID")
        FRAME_TYPE = LAST_FRAME
        data_buf[POS_FRAME_TYPE] = FRAME_TYPE
        data_buf[POS_COMMAND] = COMMAND_BL_GET_CID
        data_buf[POS_DATALEN] =  COMMAND_BL_GET_CID_DATALEN

        apply_4byte_crc(data_buf, COMMAND_BL_GET_CID_DATALEN + NUMBER_OF_HEADER_BYTES)

        Write_to_serial_port(data_buf, COMMAND_BL_GET_CID_DATALEN + NUMBER_OF_CONTROL_BYTES)

        ret_value = read_bootloader_reply(data_buf[1])

    elif(command == 3):
        print("\n   Command == > BL_GET_RDP_STATUS")
        FRAME_TYPE = LAST_FRAME
        data_buf[POS_FRAME_TYPE] = FRAME_TYPE
        data_buf[POS_COMMAND] = COMMAND_BL_GET_RDP_STATUS
        data_buf[POS_DATALEN] = COMMAND_BL_GET_RDP_STATUS_DATALEN

        apply_4byte_crc(data_buf, COMMAND_BL_GET_RDP_STATUS_DATALEN + NUMBER_OF_HEADER_BYTES)
        
        Write_to_serial_port(data_buf, COMMAND_BL_GET_RDP_STATUS_DATALEN + NUMBER_OF_CONTROL_BYTES)
        
        ret_value = read_bootloader_reply(data_buf[1])
    
    elif(command == 4):
        print("\n   Command == > BL_FLASH_ERASE")
        FRAME_TYPE  = LAST_FRAME
        data_buf[POS_FRAME_TYPE] = FRAME_TYPE
        data_buf[POS_COMMAND] = COMMAND_BL_FLASH_ERASE
        data_buf[POS_DATALEN] = COMMAND_BL_FLASH_ERASE_DATALEN
        sector_num = input("\n   Enter the sectors to be erased as a one byte bit mask (Ex: 11111100 -> erase [Sector 2, Sector 7]): ")
        sector_num = int(sector_num, 2)
        data_buf[POS_DATALEN + 1]= sector_num 

        apply_4byte_crc(data_buf, COMMAND_BL_FLASH_ERASE_DATALEN + NUMBER_OF_HEADER_BYTES)

        Write_to_serial_port(data_buf, COMMAND_BL_FLASH_ERASE_DATALEN + NUMBER_OF_CONTROL_BYTES)
        
        ret_value = read_bootloader_reply(data_buf[1])

    elif(command == 5):
            #../UserApp_Bootloader/bin/binary.bin
            print("\n   Command == > BL_MEM_WRITE")
            bytes_remaining=0
            t_len_of_file=0
            bytes_so_far_sent = 0
            len_to_read=0


            #First get the total number of bytes in the .bin file.
            file_path = input("Enter the path and the file: ")
            t_len_of_file =calc_file_len(file_path)

            #Send the total size to the Bootloader
            file_len_byte_aray = t_len_of_file.to_bytes(4, byteorder = 'little')
            FRAME_TYPE = CONSECUTIVE_FRAME
            data_buf[POS_FRAME_TYPE] = FRAME_TYPE
            data_buf[POS_COMMAND] = COMMAND_BL_MEM_WRITE
            data_buf[POS_DATALEN] = len(file_len_byte_aray)
            data_buf[NUMBER_OF_HEADER_BYTES] = file_len_byte_aray[0]
            data_buf[NUMBER_OF_HEADER_BYTES + 1] = file_len_byte_aray[1]
            data_buf[NUMBER_OF_HEADER_BYTES + 2] = file_len_byte_aray[2]
            data_buf[NUMBER_OF_HEADER_BYTES + 3] = file_len_byte_aray[3]

            apply_4byte_crc(data_buf, NUMBER_OF_HEADER_BYTES + len(file_len_byte_aray))

            Write_to_serial_port(data_buf, len(file_len_byte_aray) + NUMBER_OF_CONTROL_BYTES)

            #Wait to receive the ack containing the sectors to be erased
            ret_value = read_bootloader_reply(data_buf[1])
            if(ret_value != 0):
                print("\n Error. Host did not receive the sectors to be erased!!")
                return
            
            #Move the mem_write to the next operation (receive the erased sectors)
            mem_write_active = 1

            #Wait to receive the confirmation that the sectors have been erased
            ret_value = read_bootloader_reply(data_buf[1])
            if(ret_value != 0):
                print("\n Error. Bootloader did not erase the sectors successfuly!!")
                return

            open_the_file(file_path)

            bytes_remaining = t_len_of_file - bytes_so_far_sent

            while(bytes_remaining):
                #Move the mem_write to the next operation (send blocks of data to be flashed)
                mem_write_active = 2
                if(bytes_remaining >= PROG_BLOCK_MAX_SIZE):
                    FRAME_TYPE = CONSECUTIVE_FRAME
                    len_to_read = PROG_BLOCK_MAX_SIZE
                else:
                    FRAME_TYPE = LAST_FRAME
                    len_to_read = bytes_remaining
                #get the bytes in to buffer by reading file
                for x in range(len_to_read):
                    file_read_value = bin_file.read(1)
                    file_read_value = bytearray(file_read_value)
                    data_buf[POS_DATALEN + 1 + x]= int(file_read_value[0])

                #first field is "len_to_follow"
                data_buf[POS_FRAME_TYPE] = FRAME_TYPE
                data_buf[POS_COMMAND] = COMMAND_BL_MEM_WRITE
                data_buf[POS_DATALEN] = len_to_read

                apply_4byte_crc(data_buf, NUMBER_OF_HEADER_BYTES + len_to_read)
                

                Write_to_serial_port(data_buf, NUMBER_OF_CONTROL_BYTES + len_to_read)

                bytes_so_far_sent+=len_to_read
                bytes_remaining = t_len_of_file - bytes_so_far_sent
                print("\n   bytes_so_far_sent:{0} -- bytes_remaining:{1}\n".format(bytes_so_far_sent,bytes_remaining)) 

                ret_value = read_bootloader_reply(data_buf[1])
                if(ret_value != 0):
                    print("\n Bootloader did not manage to flash this block!")
                
            mem_write_active=0
            
    
    elif(command == 6):
        #To be implemented next : Set the protection level of each setot to WProtection, RWProtection or NoProtection
        pass

    elif(command == 7):
        print("\n   Command == > COMMAND_BL_GET_RW_SECTOR_PROT")
        FRAME_TYPE  = LAST_FRAME
        data_buf[0] = FRAME_TYPE
        data_buf[1] = COMMAND_BL_GET_RW_SECTOR_PROT
        data_buf[2] = COMMAND_BL_GET_RW_SECTOR_PROT_DATALEN

        apply_4byte_crc(data_buf, COMMAND_BL_GET_RW_SECTOR_PROT_DATALEN + NUMBER_OF_HEADER_BYTES)

        Write_to_serial_port(data_buf, COMMAND_BL_GET_RW_SECTOR_PROT_DATALEN + NUMBER_OF_CONTROL_BYTES)
        
        ret_value = read_bootloader_reply(data_buf[1])

    elif(command == 8):
        print("\n   Command == > COMMAND_BL_JMP_TO_USERAPP")
        FRAME_TYPE  = LAST_FRAME
        data_buf[0] = FRAME_TYPE
        data_buf[1] = COMMAND_BL_JMP_TO_USERAPP
        data_buf[2] = COMMAND_BL_JMP_TO_USERAPP_DATALEN

        apply_4byte_crc(data_buf, COMMAND_BL_JMP_TO_USERAPP_DATALEN + NUMBER_OF_HEADER_BYTES)

        Write_to_serial_port(data_buf, COMMAND_BL_JMP_TO_USERAPP_DATALEN + NUMBER_OF_CONTROL_BYTES)
        
        ret_value = read_bootloader_reply(data_buf[1])
        
    else:
        print("\n   Please input valid command code\n")
        return

    if ret_value == -2 :
        print("\n   TimeOut : No response from the bootloader")
        print("\n   Reset the board and Try Again !")
        return

def read_bootloader_reply(command_code):
    len_to_follow=0 
    ret = -2 

    ack=read_serial_port(3)
    if(len(ack)):
        a_array=bytearray(ack)
         
        if (a_array[1]== 0xAA):
            #CRC of last command was good .. received ACK and "len to follow"
            len_to_follow=a_array[2]
            print("\n   CRC : SUCCESS Len :",len_to_follow)
            
            if (command_code == COMMAND_BL_GET_VER) :
                process_COMMAND_BL_GET_VER(len_to_follow)
                                
            elif(command_code == COMMAND_BL_GET_CID) :
                process_COMMAND_BL_GET_CID(len_to_follow)
                
            elif(command_code == COMMAND_BL_GET_RDP_STATUS) :
                process_COMMAND_BL_GET_RDP_STATUS(len_to_follow)
                    
            elif(command_code == COMMAND_BL_FLASH_ERASE) :
                process_COMMAND_BL_FLASH_ERASE(len_to_follow)
                
            elif(command_code == COMMAND_BL_MEM_WRITE) :
                process_COMMAND_BL_MEM_WRITE(len_to_follow)

            elif(command_code == COMMAND_BL_GET_RW_SECTOR_PROT):
                process_COMMAND_BL_GET_RW_SECTOR_PROT(len_to_follow)

            elif(command_code == COMMAND_BL_JMP_TO_USERAPP) :
                process_COMMAND_BL_JMP_TO_USERAPP(len_to_follow)
                
                
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

def apply_4byte_crc(data_buf, len):
    crc32       = get_crc(data_buf, len) 
    data_buf[len] = word_to_byte(crc32,1,1) 
    data_buf[len + 1] = word_to_byte(crc32,2,1) 
    data_buf[len + 2] = word_to_byte(crc32,3,1) 
    data_buf[len + 3] = word_to_byte(crc32,4,1) 
            

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
    print("   BL_SET_RW_SECTOR_PROT                 --> 6")
    print("   BL_GET_RW_SECTOR_PROT                 --> 7")
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


def protection_type():
    pass

