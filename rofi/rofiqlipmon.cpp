#include <glib.h>

extern "C" {
#include <rofi/mode.h>
#include <rofi/helper.h>
#include <rofi/mode-private.h>
}

#include "qlipdata.h"

RofiData* RofiDataFromMode(const Mode *mode){
    return reinterpret_cast<RofiData*> (mode_get_private_data(mode));
}

static char* QStringDupa(const QString& line){
      QByteArray ba = line.toLocal8Bit();
      return g_strdup(ba.data());
}


/**
 * @param mode The mode to initialize
 *
 * Initialize mode
 *
 * @returns FALSE if there was a failure, TRUE if successful
 */
static int qlipmon_mode_init(Mode *sw) {
  if (mode_get_private_data(sw) == nullptr) {
    RofiData *data = QlipData::getEntries();
    mode_set_private_data(sw, reinterpret_cast<void *>(data));
  }
  return TRUE;
}

/**
 * @param mode The mode to query
 *
 * Get the number of entries in the mode.
 *
 * @returns an unsigned in with the number of entries.
 */
static unsigned int qlipmon_mode_get_num_entries(const Mode *sw) {
  RofiData* data = RofiDataFromMode(sw);
  if (data->error){
      return 0;
  }else{
    return data->entries.size();
  }
}

/**
 * @param mode The mode to query
 * @param menu_retv The menu return value.
 * @param input Pointer to the user input string. [in][out]
 * @param selected_line the line selected by the user.
 *
 * Acts on the user interaction.
 *
 * @returns the next #ModeMode.
 */
static ModeMode qlipmon_mode_result(
        Mode *sw,
        int mretv,
        char **input,
        unsigned int selected_line
        ) {
    ModeMode retv = MODE_EXIT;
    Q_UNUSED( sw )
    Q_UNUSED( input )

    RofiData* data = RofiDataFromMode(sw);


    if (mretv & MENU_NEXT) {
        retv = NEXT_DIALOG;
    } else if (mretv & MENU_PREVIOUS) {
        retv = PREVIOUS_DIALOG;
    } else if (mretv & MENU_QUICK_SWITCH) {
        retv = (ModeMode) (mretv & MENU_LOWER_MASK);
    } else if ((mretv & MENU_OK) ) {
        if(!data->error){
            const QString selected = data->entries.value(selected_line);
            qDebug()<<"Selected = " << selected;
            QlipData::setText(selected);
        }
        retv = MODE_EXIT;
    } else if ((mretv & MENU_ENTRY_DELETE) == MENU_ENTRY_DELETE) {
        retv = RELOAD_DIALOG;
    }
    return retv;
}

/**
 * @param mode The mode to destroy
 *
 * Destroy the mode
 */
static void qlipmon_mode_destroy(Mode *sw) {
  RofiData* data = RofiDataFromMode(sw);
  if (data != nullptr) {
      delete  data;
  }
}

/**
 * @param mode The mode to query
 *
 * Query the mode for a user display.
 *
 * @return a new allocated (valid pango markup) message to display (user should
 * free).
 */
static char *qlipmon_get_message(const Mode *sw) {
  RofiData* data = RofiDataFromMode(sw);
  if(data->error){
      QString line = QString("<b>QlipMon <i>Error:</i> </b>");
      line += QString("<i>") + data->errorString + QString("</i>");
      return QStringDupa(line);
  }else if( 0 == data->entries.size() ){
      QString line = QString("<b>QlipMon:</b> <i>No clipboard history!</i>");
      return QStringDupa(line);
  }else{
      return NULL;
  }
}

/**
 * @param mode The mode to query
 * @param selected_line The entry to query
 * @param state The state of the entry [out]
 * @param attribute_list List of extra (pango) attribute to apply when displaying. [out][null]
 * @param get_entry If the should be returned.
 *
 * Returns the string as it should be displayed for the entry and the state of how it should be displayed.
 *
 * @returns allocated new string and state when get_entry is TRUE otherwise just the state.
 */
static char *get_display_value(
  const Mode *sw,
  unsigned int selected_line,
  int *state,
  GList **attr_list,
  int get_entry
) {
  Q_UNUSED( get_entry )
  Q_UNUSED( state )
  Q_UNUSED( attr_list )
  RofiData* data = RofiDataFromMode(sw);

  // Rofi is not yet exporting these constants in their headers
  // *state |= MARKUP;
  // https://github.com/DaveDavenport/rofi/blob/79adae77d72be3de96d1c4e6d53b6bae4cb7e00e/include/widgets/textbox.h#L104
  //*state |= 8;

  if(data->error){
      return NULL;
  }else{
      QString line = data->entries.value(selected_line);
      line.replace("\n", "⏎");
      return QStringDupa(line);
  }

}

/**
 * @param sw The mode object.
 * @param tokens The tokens to match against.
 * @param index  The index in this plugin to match against.
 *
 * Match the entry.
 *
 * @param returns try when a match.
 */
static int qlipmon_token_match(const Mode *sw, rofi_int_matcher **tokens, unsigned int index) {
  RofiData* data = RofiDataFromMode(sw);

  if(data->error){
      // always display error message.
      // Mute since now no message is displayed in the ellement list
      // error message is displayed in the status bar
      return TRUE;
  }else{
      QByteArray ba = data->entries.value(index).toLocal8Bit();
      return helper_token_match(tokens, ba.data());
  }
}

cairo_surface_t * qlipmon_get_icon ( const Mode *mode, unsigned int selected_line, int height ){
    Q_UNUSED( mode )
    Q_UNUSED( selected_line )
    Q_UNUSED( height )
    return nullptr;
}

static char _name[] = "qlipmon";

G_MODULE_EXPORT Mode mode = {
  .abi_version        = ABI_VERSION,
  .name               = _name,
  .cfg_name_key       = {},
  //uncommenting this results in segfault
  .display_name       = {},
  ._init              = qlipmon_mode_init,
  ._destroy           = qlipmon_mode_destroy,
  ._get_num_entries   = qlipmon_mode_get_num_entries,
  ._result            = qlipmon_mode_result,
  ._token_match       = qlipmon_token_match,
  ._get_display_value = get_display_value,
  ._get_icon          = nullptr,
  ._get_completion    = nullptr,
  ._preprocess_input  = nullptr,
  ._get_message       = qlipmon_get_message,
  .private_data       = nullptr,
  .free               = nullptr,
  .ed                 = {},
  .module             = {},
};
