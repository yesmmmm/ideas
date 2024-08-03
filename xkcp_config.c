#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <syslog.h>
#include <json-c/json.h>

#include "debug.h"
#include "xkcp_config.h"

static struct xkcp_config config;

struct xkcp_config * xkcp_get_config(void)
{
	return &config;
}

struct xkcp_param *xkcp_get_param(void)
{
	return &config.param;
}

void config_init()
{
	config.daemon = 1;
}

static void xkcp_param_free(struct xkcp_param *param)
{
	if (param->local_interface)
		free(param->local_interface);

	if (param->remote_addr)
		free(param->remote_addr);

	if (param->key)
		free(param->key);

	if (param->crypt)
		free(param->crypt);

	if (param->mode)
		free(param->mode);
}

int xkcp_parse_param(const char *filename)
{
	return xkcp_parse_json_param(&config.param, filename);
}

// 1: error; 0, success
int xkcp_parse_json_param(struct xkcp_param *param, const char *filename)
{
	if (!param)
		return 1;

	int nret = 0;
	struct json_object *json_config = json_object_from_file(filename);
	if (!json_config || !json_object_is_type(json_config, json_type_object)) {
		debug(LOG_ERR, "json_object_from_file [%s] failed", filename); 
		return 1;	
	}

	struct json_object *j_obj = NULL;
	if (json_object_object_get_ex(json_config, "localinterface", &j_obj)) {
		param->local_interface = strdup(json_object_get_string(j_obj));
	} else
		param->local_interface = strdup("br-lan");
	
	if (json_object_object_get_ex(json_config, "localport", &j_obj)) {
		param->local_port = json_object_get_int(j_obj);
	} else
		param->local_port = 9088;

	if (json_object_object_get_ex(json_config, "remoteaddr", &j_obj)) {
		param->remote_addr = strdup(json_object_get_string(j_obj));
	} else {
		nret = 1;
		goto err;
	}

	if (json_object_object_get_ex(json_config, "remoteport", &j_obj)) {
		param->remote_port = json_object_get_int(j_obj);
	} else
		param->remote_port = 9089;

	if (json_object_object_get_ex(json_config, "key", &j_obj)) {
		param->key = strdup(json_object_get_string(j_obj));
	}

	if (json_object_object_get_ex(json_config, "crypt", &j_obj)) {
		param->crypt = strdup(json_object_get_string(j_obj));
	}

	if (json_object_object_get_ex(json_config, "mode", &j_obj)) {
		param->mode = strdup(json_object_get_string(j_obj));
	}

	if (json_object_object_get_ex(json_config, "conn", &j_obj)) {
		param->conn = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "autoexpire", &j_obj)) {
		param->auto_expire = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "scavengettl", &j_obj)) {
		param->scavenge_ttl = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "mtu", &j_obj)) {
		param->mtu = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "sndwnd", &j_obj)) {
		param->sndwnd = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "rcvwnd", &j_obj)) {
		param->rcvwnd = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "datashard", &j_obj)) {
		param->data_shard = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "parity_shard", &j_obj)) {
		param->parity_shard = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "dscp", &j_obj)) {
		param->dscp = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "nocomp", &j_obj)) {
		param->nocomp = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "acknodelay", &j_obj)) {
		param->ack_nodelay = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "nodelay", &j_obj)) {
		param->nodelay = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "interval", &j_obj)) {
		param->interval = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "resend", &j_obj)) {
		param->resend = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "nc", &j_obj)) {
		param->nc = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "sockbuf", &j_obj)) {
		param->sock_buf = json_object_get_int(j_obj);
	}

	if (json_object_object_get_ex(json_config, "keepalive", &j_obj)) {
		param->keepalive = json_object_get_int(j_obj);
	}

err:
	json_object_put(json_config);
	if (nret) xkcp_param_free(param);
	return nret;
}
