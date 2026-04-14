#include "date_display.h"
#include <string.h>

// ── Position & size ───────────────────────────────────────────────────────────
// Positioned just below the clock — mirror its X/W and use the next font size down.

#if defined(PBL_PLATFORM_EMERY)
#define DATE_X 0
#define DATE_Y 205
#define DATE_W 200
#define DATE_H 26
#define DATE_FONT RESOURCE_ID_FONT_ANIMEACE_18

#elif defined(PBL_PLATFORM_GABBRO)
#define DATE_X 0
#define DATE_Y 220
#define DATE_W 260
#define DATE_H 26
#define DATE_FONT RESOURCE_ID_FONT_ANIMEACE_18

#elif defined(PBL_PLATFORM_CHALK)
#define DATE_X 0
#define DATE_Y 153
#define DATE_W 180
#define DATE_H 22
#define DATE_FONT RESOURCE_ID_FONT_ANIMEACE_14

#elif defined(PBL_PLATFORM_BASALT)
#define DATE_X 0
#define DATE_Y 150
#define DATE_W 144
#define DATE_H 22
#define DATE_FONT RESOURCE_ID_FONT_ANIMEACE_14

#else // aplite, diorite, flint
#define DATE_X 64
#define DATE_Y 145
#define DATE_W 80
#define DATE_H 20
#define DATE_FONT RESOURCE_ID_FONT_ANIMEACE_12
#endif

// ── Locale-aware day/month names ──────────────────────────────────────────────
// strftime %a/%b on Pebble always outputs English — we resolve names manually.
// Abbreviations are ASCII-only (accented chars may not render in custom fonts).

typedef struct {
  const char *prefix;    // first 2 chars of i18n_get_system_locale(), e.g. "fr"
  const char *days[7];   // Sun=0 .. Sat=6
  const char *months[12]; // Jan=0 .. Dec=11
} LocaleStrings;

static const LocaleStrings LOCALES[] = {
  { "fr",
    {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"},
    {"Jan", "Fev", "Mar", "Avr", "Mai", "Jun", "Jul", "Aou", "Sep", "Oct", "Nov", "Dec"} },
  { "de",
    {"So",  "Mo",  "Di",  "Mi",  "Do",  "Fr",  "Sa"},
    {"Jan", "Feb", "Mrz", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"} },
  { "es",
    {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"},
    {"Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"} },
  { "it",
    {"Dom", "Lun", "Mar", "Mer", "Gio", "Ven", "Sab"},
    {"Gen", "Feb", "Mar", "Apr", "Mag", "Giu", "Lug", "Ago", "Set", "Ott", "Nov", "Dic"} },
  { "pt",
    {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"},
    {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez"} },
  // English — also serves as fallback for unsupported locales
  { "en",
    {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"},
    {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"} },
};
#define LOCALE_COUNT ((int)ARRAY_LENGTH(LOCALES))

static const LocaleStrings *get_locale(void)
{
  const char *sys = i18n_get_system_locale();
  for (int i = 0; i < LOCALE_COUNT - 1; i++)
    if (strncmp(sys, LOCALES[i].prefix, 2) == 0)
      return &LOCALES[i];
  return &LOCALES[LOCALE_COUNT - 1]; // English fallback
}

// ── Layer ─────────────────────────────────────────────────────────────────────

static TextLayer *s_layer;
static char s_buf[12];

void date_display_create(Layer *root)
{
  s_layer = text_layer_create(GRect(DATE_X, DATE_Y, DATE_W, DATE_H));
  text_layer_set_text_alignment(s_layer, GTextAlignmentCenter);
  text_layer_set_font(s_layer, fonts_load_custom_font(resource_get_handle(DATE_FONT)));
  text_layer_set_background_color(s_layer, GColorClear);
  text_layer_set_text_color(s_layer, GColorBlack);
  layer_add_child(root, text_layer_get_layer(s_layer));
}

void date_display_destroy(void)
{
  text_layer_destroy(s_layer);
}

void date_display_update(struct tm *t)
{
  const LocaleStrings *loc = get_locale();
  snprintf(s_buf, sizeof(s_buf), "%s %02d %s",
           loc->days[t->tm_wday], t->tm_mday, loc->months[t->tm_mon]);
  text_layer_set_text(s_layer, s_buf);
}
