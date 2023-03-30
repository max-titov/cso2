#include "lab.h"

static int counter = 0;
/* forward function for "attack 0" case.
   change this code to implement your attack
 */
void forward_attack_0(struct message *message) {
	if (counter==0) {
		counter++;
		send_message(new_message(
			'A', 'B',
			"PAY $10000000 TO M.",
			false, false
		));
	}
	send_message(message);
}
/* forward function for "attack 1" case.
   change this code to implement your attack
 */
void forward_attack_1(struct message *message) {
	if (counter==0) {
		counter++;
		send_message(new_message(
			'A', 'B',
			"PAY $10000000 TO M.",
			true, false
		));
	}
	send_message(message);
}

/* forward function for "attack 2" case.
   change this code to implement your attack
 */
void forward_attack_2(struct message *message) {
	send_message(message);
    send_message(message);
}

/* forward function for "attack 3" case.
   change this code to implement your attack
 */
void forward_attack_3(struct message *message) {
    send_message(message);
}

/* forward function for "attack 4" case.
   I did not intend this one to be possible. */
void forward_attack_4(struct message *message) {
    send_message(message);
}
