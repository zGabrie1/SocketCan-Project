#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#define CAN_INTERFACE "can0" // Nome da interface vcan

int main(void) {
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    int s, nbytes;

    // Cria um socket CAN
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Erro ao abrir o socket CAN");
        return 1;
    }

    // Configura a interface CAN
    strcpy(ifr.ifr_name, CAN_INTERFACE);
    ioctl(s, SIOCGIFINDEX, &ifr); // system call

    // Associa o socket com a interface
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Erro ao associar o socket com a interface");
        close(s);
        return 1;
    }

    while(1) {
        nbytes = read(s, &frame, sizeof(struct can_frame));
        if(nbytes < 0) {
            perror("CAN frame read error");
            return -3;
        }

        printf("Receided CAN frame - ID: 0x%X Data: ", frame.can_id);
        for(int i=0; i < frame.can_dlc; i++) {
            printf("%02X ", frame.data[i]);
        }
        printf("\n");
    }
}


/*--------Como editar o can frame--------
int main(void) {
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    int s;

    // Cria um socket CAN
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Erro ao abrir o socket CAN");
        return 1;
    }

    // Configura a interface CAN
    strcpy(ifr.ifr_name, CAN_INTERFACE);
    ioctl(s, SIOCGIFINDEX, &ifr); // system call

    // Associa o socket com a interface
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Erro ao associar o socket com a interface");
        close(s);
        return 1;
    }

    // Configura os parâmetros da mensagem CAN
    frame.can_id = 0x123;  // ID da mensagem CAN
    frame.can_dlc = 2;     // Comprimento dos dados
    frame.data[0] = 0x11;  // Dados
    frame.data[1] = 0x22;  // Dados

    // Envia a mensagem CAN
    if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Erro ao enviar a mensagem CAN");
        close(s);
        return 1;
    }

    printf("Mensagem CAN enviada com sucesso!\n");

    // Fecha o socket CAN
    close(s);

    return 0;
}

*/