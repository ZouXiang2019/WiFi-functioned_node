#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include <esp_http_server.h>
#include "rom/ets_sys.h"

//#define CONFIG_PACKET_RATE 1000

char *data11 = "1\n";

// void set_data11(char *data_set)
// {
//     strcpy(data11, data_set);
// }


void socket_transmitter_sta_loop11() {
    int socket_fd = -1;
    while (1) {
        close(socket_fd);
        char *ip = "192.168.4.11";
        struct sockaddr_in caddr;
        caddr.sin_family = AF_INET;
        caddr.sin_port = htons(2223);
        /*while (!is_wifi_connected()) {
            // wait until connected to AP
            printf("waiting\n");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }*/
        if (inet_aton(ip, &caddr.sin_addr) == 0) {
            printf("ERROR: inet_aton\n");
            continue;
        }

        socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
        if (socket_fd == -1) {
            printf("ERROR: Socket creation error [%s]\n", strerror(errno));
            continue;
        }
        if (connect(socket_fd, (const struct sockaddr *) &caddr, sizeof(struct sockaddr)) == -1) {
            printf("ERROR: socket connection error [%s]\n", strerror(errno));
            continue;
        }

        while (1) {
            /*if (!is_wifi_connected()) {
                printf("ERROR: wifi is not connected\n");
                break;
            }*/

            if (sendto(socket_fd, &data11, strlen(data11), 0, (const struct sockaddr *) &caddr, sizeof(caddr)) !=
                strlen(data11)) {
                vTaskDelay(1);
                continue;
            }
            vTaskDelay(2);

#ifdef CONFIG_PACKET_RATE
            vTaskDelay(CONFIG_PACKET_RATE != 0 ? floor(1000 / CONFIG_PACKET_RATE) : 0);
            ets_delay_us(((1000.0 / CONFIG_PACKET_RATE) - floor(1000 / CONFIG_PACKET_RATE)) * 1000);
#else
            vTaskDelay(10); // This limits TX to approximately 100 per second.
#endif
        }
    }
}
