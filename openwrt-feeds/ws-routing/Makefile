# Copyright (C) 2010 Commotion
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
include $(TOPDIR)/rules.mk

PKG_NAME:=ws-routing
PKG_RELEASE:=1
PKG_VERSION:=0.0.1

PKG_BUILD_DIR:=$(BUILD_DIR)/commotion-ws-routing

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git@github.com:AdamMagaluk/commotion-ws-routing.git
PKG_SOURCE_VERSION:=68561697f43272e7f660b827f8c7b742226cc59f
PKG_SOURCE_SUBDIR:=commotion-ws-routing
PKG_SOURCE_PROTO:=git

include $(INCLUDE_DIR)/package.mk

define Package/ws-routing
  SECTION:=commotion-apps
  CATEGORY:=Commotion-apps
  TITLE:=Commotion websocket routing using libwebsockets
  DEPENDS:= +libwebsockets +jansson
  URL:=https://github.com/AdamMagaluk/commotion-ws-routing
endef

define Build/Configure
	$(call Build/Configure/Default,--with-linux-headers=$(LINUX_DIR) --datadir=/usr/share)
endef

define Package/ws-routing/description
  Commotion websocket routing using libwebsockets
endef




define Package/ws-routing/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/.libs/wsroutingserver $(1)/usr/sbin/

	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/.libs/libcommotionws.{a,so*} $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/www/luci-static/resources/
	$(CP) -a $(PKG_BUILD_DIR)/js/* $(1)/www/luci-static/resources/ 2>/dev/null || true

	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/commotion
	$(CP) -a $(PKG_BUILD_DIR)/tests/luci-chat-server/* $(1)/usr/lib/lua/luci/view/commotion/ 2>/dev/null || true
	
endef

$(eval $(call BuildPackage,ws-routing))
