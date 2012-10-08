// SVN: $Revision$
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>

class encrypt_stream_aes : public encrypt_stream
{
private:
	CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption *enc;
	CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption *dec;

public:
	encrypt_stream_aes();
	~encrypt_stream_aes();

	int get_ivec_size();
	int get_key_size();

	bool init(unsigned char *key, int key_len, unsigned char *ivec, bool force=false);

	std::string get_name();

	void encrypt(unsigned char *p_in, size_t len, unsigned char *p_out);
	void decrypt(unsigned char *p_in, size_t len, unsigned char *p_out);
};
