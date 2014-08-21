/*
 * l7f.c
 * (C) 2014, all rights reserved,
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * DESCRIPTION:
 * This is a simple layer7 sniffer which demonstrates how to use the windivert
 * layer7 filtering extension.
 *
 * usage: l7f.exe windivert-filter layer7-filtering sniff|grab [priority]
 *
 */

#include "windivert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int can_exit = 0;

void __cdecl sigint_watchdog(int signo) {
    can_exit = 1;
}

int __cdecl main(int argc, char **argv) {
    WINDIVERT_ADDRESS addr;
    UINT buf_len, b, writelen;
    HANDLE filter, console;
    UINT64 flags = WINDIVERT_FLAG_SNIFF;
    INT16 priority = 0;
    unsigned char cap_buffer[65535];
    if (argc >= 3) {
        if (argc >= 4) {
            if (strcmp(argv[3], "sniff") == 0) {
                flags = WINDIVERT_FLAG_SNIFF;
            } else if (strcmp(argv[3], "grab") == 0) {
                flags = 0;
            } else {
                printf("error: unknown flag \"%s\"\n", argv[3]);
                exit(EXIT_FAILURE);
            }
        }
        if (argc >= 5) {
            priority = (UINT)atoi(argv[4]);
        }
        filter = WinDivertOpenLayer7SubFilterEx(argv[1], argv[2], WINDIVERT_LAYER_NETWORK, priority, flags);
        if (filter == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_INVALID_PARAMETER)
            {
                fprintf(stderr, "error: filter syntax error\n");
                exit(EXIT_FAILURE);
            }
            fprintf(stderr, "error: failed to open the WinDivert device (%d)\n",
                GetLastError());
            exit(EXIT_FAILURE);
        }

        signal(SIGINT, sigint_watchdog);
        signal(SIGTERM, sigint_watchdog);
        while (!can_exit) {
            if (!WinDivertRecv(filter, cap_buffer, sizeof(cap_buffer), &addr, &buf_len))
            {
                Sleep(10);
                continue;
            }

            for (b = 0; b < buf_len; b++) {
                if ((b % 40) == 0) {
                    printf("\n\t");
                }
                if (isprint(cap_buffer[b])) {
                    printf("%c", cap_buffer[b]);
                } else {
                    printf(".");
                }
            }
            printf("\n");
            if (flags == 0) {
                WinDivertSend(filter, cap_buffer, buf_len, &addr, &writelen);
            }
            Sleep(10);
        }
        WinDivertClose(filter);
    } else {
        printf("usage: l7f.exe windivert-filter layer7-filtering sniff|grab [priority]\n"
               "       l7f \"outbound and true\" \"\\\"GET*HTTP/1*\\\" or \\\"POST*HTTP/1\\\"");
    }
    return 0;
}
