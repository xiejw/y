#!/bin/sh
#
# must run under non-`root`
#

set -x
set -e

Y_BIN=~/Workspace/y/dotfiles/bin/
NGINX_FMT=${Y_BIN}/nginxfmt
NGINX_CONF=/etc/nginx/nginx.conf
NGINX_CONF_TMP=/etc/nginx/nginx.conf.tmp

sudo certbot --renew-by-default
which ${NGINX_FMT} || (echo "no nginxfmt" && exit 1)
sudo bash -c "${NGINX_FMT} < ${NGINX_CONF} > ${NGINX_CONF_TMP}"
sudo mv ${NGINX_CONF_TMP} ${NGINX_CONF}
sudo systemctl restart nginx

