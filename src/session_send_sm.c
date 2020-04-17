#include "./../include/session_send_sm.h"

signed_word MESS_idle_handler(struct SessionInfo* session, struct MESS_ctx* ctx) {
	/* In the future, check for input and store it */
	return 0;
}

signed_word MESS_encrypt_handler(struct SessionInfo* session, struct MESS_ctx* ctx) {
	uint8_t IV[AEGIS_IV_NB];

	if (!ctx->inputDataValid)
		return 0;

	/* Encode and assume input data sits at correct position */


	/* Encrypt */
	getRandomBytes(AEGIS_IV_NB, IV);
	session->aegisCtx.iv = IV;

}


messState messSendContinue(struct SessionInfo* session, struct MESS_ctx* ctx, messState currentState) {
	word input;

	switch (currentState)
	{
	case MESS_idle:
		return MESS_idle_handler(session, ctx) ? MESS_encrypt : MESS_idle;

	case MESS_encrypt:
		return MESS_encrypt_handler(session, ctx) ? MESS_send : MESS_idle;


	default:
		break;
	}
}