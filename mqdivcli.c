#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int msgget(key_t key, IPC_PRIVATE);

int msgsnd(int msg_id, const void *buf, siye_t m_len, int flag);

struct mz_nachricht {

   long nachrichten_typ; // Message-typ
   
     char nachricht[MSG_LEN] // Message Text
}
