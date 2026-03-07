#!/bin/sh
#
# Auto renew cert and restart nginx.
#
# NOTE: must run under non-`root`
#

set -x
set -e

NGINX_FMT="go run ${HOME}/Workspace/y/tools/scripts/nginxfmt.go"
NGINX_CONF=/etc/nginx/nginx.conf
NGINX_CONF_TMP=/etc/nginx/nginx.conf.tmp

sudo certbot --renew-by-default
sudo bash -c "${NGINX_FMT} < ${NGINX_CONF} > ${NGINX_CONF_TMP}"
sudo mv ${NGINX_CONF_TMP} ${NGINX_CONF}
sudo systemctl restart nginx

