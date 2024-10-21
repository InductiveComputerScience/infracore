static _Bool sendAll(int s, uint8_t *data, int64_t len){
	_Bool success;
	int64_t sent, tosend, ret;

	sent = 0;
	tosend = len;

	success = true;

	while(sent < tosend && success){
		ret = send(s, data + sent, len - sent, 0);
		if(ret != -1){
			sent += ret;
		}else{
			success = false;
		}
	}

	return success;
}

static _Bool recvAll(int s, uint8_t *data, int64_t len){
	_Bool success;
	int64_t recved, torecv, ret;

	recved = 0;
	torecv = len;

	success = true;

	while(recved < torecv && success){
		ret = recv(s, data + recved, len - recved, 0);
		if(ret == 0){
			success = false;
		}else if(ret != -1){
			recved += ret;
		}else{
			success = false;
		}
	}

	return success;
}
