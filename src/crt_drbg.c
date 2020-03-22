#include "./../include/crt_drbg.h"

void CTR_DRBG_Update(const uint8_t *providedData, struct CTR_DRBG_ctx *state) {
    uint8_t temp[seedlen];
    
    add_overflow(state->V, one, state->V);
    

}

void CTR_DRBG_Instantiate(const uint8_t *entropyInput, const word entroptyInputLength, const uint8_t *nonce, const word nonceLength,
                          const uint8_t *personalizationString, const word personalizationStringLength, struct CTR_DRBG_ctx *state) {
    uint8_t dfInput[]
}

void CTR_DRBG_Reseed(const CTR_DRBG_ctx *workingState, const uint8_t *entropyInput) {

}

void CTR_DRBG_Generate(struct CTR_DRBG_ctx* workingState, word requestedNbBits) {

}