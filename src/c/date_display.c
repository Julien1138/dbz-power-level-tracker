#include "date_display.h"

static char s_buf[16] = "";

void date_display_update(struct tm *t)
{
  strftime(s_buf, sizeof(s_buf), "%b %e %Y", t);
}

const char *date_display_get_text(void)
{
  return s_buf;
}
