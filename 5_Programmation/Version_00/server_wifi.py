import network

try:
  import usocket as socket
except:
  import socket
  
import select
  
# enable station interface and connect to WiFi access point
nic = network.WLAN(network.AP_IF)
nic.active(True)
#nic.connect('your-ssid', 'your-password')
# now use sockets as usual
nic.config(essid='FOR', password=b"123456789", channel=11,authmode=network.AUTH_WPA_WPA2_PSK)

while nic.active() == False:
  pass

print('Connection successful')
print(nic.ifconfig())

class Communication():
    def __init__(self, host, port):
        self.host = host
        self.port = port

    def create_server(self):
        connexion_principale = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connexion_principale.bind((self.host, self.port))
        connexion_principale.listen(5)
        self.inout = [connexion_principale]

        return connexion_principale

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
        msg = msg.encode() #UTF_8
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
        read_sockets, write_sockets, error_sockets = select.select(self.inout, [], [], 5)
        for client in read_sockets:
            # Client est de type socket
            msg_recu = client.recv(1024)
            # Peut planter si le message contient des caractères spéciaux
            msg_recu = msg_recu.decode()
            print("Reçu {}".format(msg_recu))

        return 1
    
if __name__ == '__main__':

    c = Communication("", 12800)
    connexion_principale = c.create_server()

    serveur_lance = True
    clients_connectes = []
    
    while serveur_lance:

        connexions_demandees, wlist, xlist = select.select(c.inout, [], [], 0.05)

        for connexion in connexions_demandees:
            connexion_avec_client, infos_connexion = connexion.accept()
            # On ajoute le socket connecté à la liste des clients
            clients_connectes.append(connexion_avec_client)

        clients_a_lire = []
        try:
            clients_a_lire, wlist, xlist = select.select(clients_connectes, [], [], 0.05)
        except select.error:
            pass
        else:
            # On parcourt la liste des clients à lire
            for client in clients_a_lire:
                # Client est de type socket
                try:
                    msg_recu = client.recv(1024)
                    # Peut planter si le message contient des caractères spéciaux
                    print("Reçu {}".format(msg_recu))

                    client.send(b"5 / 5 asd")
                except :
                    vec_serveur = None
                    while connexion_principale is None:
                        connexion_principale = c.connection_server()

    print("Fermeture des connexions")

    for client in clients_connectes:
        client.close()

    connexion_principale.close()