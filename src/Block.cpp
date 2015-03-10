/*
 callblocker - blocking unwanted calls from your home phone
 Copyright (C) 2015-2015 Patrick Ammann <pammann@gmx.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "Block.h" // API

#include "Logger.h"


Block::Block() {
  m_whitelists = new FileLists(SYSCONFDIR "/" PACKAGE_NAME "/whitelists");
  m_blacklists = new FileLists(SYSCONFDIR "/" PACKAGE_NAME "/blacklists");
}

Block::~Block() {
  Logger::debug("~Block...");

  delete m_whitelists;
  m_whitelists = NULL;

  delete m_blacklists;
  m_blacklists = NULL;
}

void Block::run() {
  m_whitelists->run();
  m_blacklists->run();
}

bool Block::isNumberBlocked(enum SettingBlockMode blockMode, const std::string& rNumber, std::string* pMsg) {
  std::string reason;
  std::string msg;
  bool block;
  switch (blockMode) {
    default:
      Logger::warn("invalid block mode %d", blockMode);
    case LOGGING_ONLY:
      block = false;
      if (isWhiteListed(rNumber, &msg)) {
        reason = "found in whitelist ("+msg+"), but mode is logging only";
        break;
      }
      if (isBlacklisted(rNumber, &msg)) {
        reason = "found in blacklist ("+msg+"), but mode is logging only";
        break;
      }
      reason = "";
      break;

    case WHITELISTS_ONLY:
      if (isWhiteListed(rNumber, &msg)) {
        reason = "found in whitelist ("+msg+")";
        block = false;
      }
      reason = "";
      block = true;
      break;

    case WHITELISTS_AND_BLACKLISTS:
      if (isWhiteListed(rNumber, &msg)) {
        reason = "found in whitelist ("+msg+")";
        block = false;
        break;
      }
      if (isBlacklisted(rNumber, &msg)) {
        reason = "found in blacklist ("+msg+")";
        block = true;
        break;
      }
      reason = "";
      block = false;
      break;

    case BLACKLISTS_ONLY:
      if (isBlacklisted(rNumber, &msg)) {
        reason = "found in blacklist ("+msg+")";
        block = true;
        break;
      }
      reason = "";
      block = false;
      break;
  }

  std::string res = "Incoming call from ";
  res += rNumber;
  if (block) {
    res += " is blocked";
  }
  if (reason.length() > 0) {
    res += " (";
    res += reason;
    res += ")";
  }
  *pMsg = res;

  return block;
}

bool Block::isWhiteListed(const std::string& rNumber, std::string* pMsg) {
  return m_whitelists->isListed(rNumber, pMsg);
}

bool Block::isBlacklisted(const std::string& rNumber, std::string* pMsg) {
  return m_blacklists->isListed(rNumber, pMsg);
}

