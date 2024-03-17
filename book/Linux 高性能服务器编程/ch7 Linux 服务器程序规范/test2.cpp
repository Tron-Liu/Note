/**
 * @file test2.cpp
 * @brief 切换用户
 * @version 0.1
 * @date 2024-03-16
 */

#include <unistd.h>

static bool switch_to_user(uid_t userid, gid_t gpid) {
  // 确保目标用户不是 root
  if ((userid == 0) && (gpid == 0)) return false;

  // 确保当前用户是合法用户：root 或者目标用户
  gid_t gid = getgid();
  uid_t uid = getuid();

  if(((gid != 0) || (uid != 0)) && ((gid != gpid) || (uid != userid))) {
    return false;
  }

  // 如果不是 root，则已经是目标用户
  if (uid != 0) return true;

  // 切换到目标用户
  if ((setgid(gpid) < 0) || (setuid(userid) < 0))
    return false;

  return true;
}