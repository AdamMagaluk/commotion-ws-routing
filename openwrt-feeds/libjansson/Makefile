# Copyright (C) 2010 Commotion
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
include $(TOPDIR)/rules.mk

PKG_NAME:=jansson
PKG_RELEASE:=1
PKG_VERSION:=2.3.1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.digip.org/jansson/releases/
PKG_MD5SUM:=28848fbc02d7657d930f97ce74f80d93

include $(INCLUDE_DIR)/package.mk

define Package/jansson
  SECTION:=commotion-apps
  CATEGORY:=Commotion-apps
  TITLE:=C JSON Lib
  DEPENDS:=
  URL:=http://www.digip.org/jansson/
endef

define Build/Configure
	$(call Build/Configure/Default,--with-linux-headers=$(LINUX_DIR) --datadir=/usr/share)
endef

define Package/jansson/description
  Jansson is a C library for encoding, decoding and manipulating JSON data.
endef


define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/jacnsson.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/.libs/libjansson.{a,so*} $(1)/usr/lib/	
endef

define Package/jansson/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/.libs/libjansson.{a,so*} $(1)/usr/lib/
endef

$(eval $(call BuildPackage,jansson))
