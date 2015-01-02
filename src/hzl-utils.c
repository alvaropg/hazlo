#include <config.h>

#include "hzl-utils.h"

const gchar *
hzl_utils_get_resource_filename (void)
{
        if (g_file_test ("./data/hazlo.gresource", G_FILE_TEST_EXISTS) == TRUE)
		return "./data/hazlo.gresource";
	else
		return PACKAGE_DATA_DIR"/hzl/hazlo.gresource";
}

