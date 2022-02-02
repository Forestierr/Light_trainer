import network

try:
  import usocket as socket
except:
  import socket
  
import select
import time

# enable station interface and connect to WiFi access point
nic = network.WLAN(network.STA_IF)
nic.active(True)
nic.connect('FOR', b'123456789')

while nic.isconnected() == False:
    pass

print(nic.ifconfig())


class Communication():
    def __init__(self, host, port):
        self.host = host
        self.port = port

    def connection_server(self):
        connexion_avec_serveur = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        try :
            connexion_avec_serveur.connect((self.host, self.port))
            self.inout = [connexion_avec_serveur]
            return connexion_avec_serveur
        except OSError as msg_error:
            #print("Error : ", msg_error)
            return None

    def send_msg(self, msg):
        # Peut planter si vous tapez des caractères spéciaux
        try :
            msg = msg.encode() #UTF_8
        except : pass
        # On envoie le message
        try:
            connexion_avec_serveur.send(msg)
            return 1
        except OSError as msg_error:
            #print("Error : ", msg_error)
            return 0

    def read_msg(self):
        # Blocant !!
        try:
            msg_recu = connexion_avec_serveur.recv(1024)
            return msg_recu
        except OSError as msg_error:
            #print("Error : ", msg_error)
            return 0

    def read_all_msg(self):
        #non blocant
        read_sockets, write_sockets, error_sockets = select.select(self.inout, self.inout, [], 5)
        for client in read_sockets:
            # Client est de type socket
            msg_recu = client.recv(1024)
            # Peut planter si le message contient des caractères spéciaux
            msg_recu = msg_recu.decode()
            print("Reçu {}".format(msg_recu))

        return 1

if __name__ == '__main__':

    c = Communication("192.168.4.1", 12800)

    connexion_avec_serveur = None

    while connexion_avec_serveur is None:
        connexion_avec_serveur = c.connection_server()
        
    print("Connexion établie avec le serveur sur le port {}".format(c.port))
    
    message = b"test 1234 $"
    
    while connexion_avec_serveur is not None:

        time.sleep(1)

        res = c.send_msg(message)
        
        if not res:
            connexion_avec_serveur = None
            while connexion_avec_serveur is None:
                connexion_avec_serveur = c.connection_server()

        res = c.read_all_msg()
        
        if not res:
            connexion_avec_serveur = None
            while connexion_avec_serveur is None:
                connexion_avec_serveur = c.connection_server()

print("Fermeture de la connexion")
connexion_avec_serveur.close()