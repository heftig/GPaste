/*
 *      This file is part of GPaste.
 *
 *      Copyright 2015 Marc-Antoine Perennou <Marc-Antoine@Perennou.com>
 *
 *      GPaste is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      GPaste is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with GPaste.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gpaste-ui-window.h>
#include <gpaste-client.h>

#include <glib/gi18n.h>

#include <stdlib.h>

#define ACTION_DATA(index) ((gpointer *) user_data)[index]

static void
about_activated (GSimpleAction *action    G_GNUC_UNUSED,
                 GVariant      *parameter G_GNUC_UNUSED,
                 gpointer       user_data)
{
    g_paste_client_about (G_PASTE_CLIENT (ACTION_DATA (1)), NULL, NULL);
}

static void
quit_activated (GSimpleAction *action    G_GNUC_UNUSED,
                GVariant      *parameter G_GNUC_UNUSED,
                gpointer       user_data)
{
    g_application_quit (G_APPLICATION (ACTION_DATA (0)));
}

static void
show_win (GApplication *application)
{
    for (GList *wins = gtk_application_get_windows (GTK_APPLICATION (application)); wins; wins = g_list_next (wins))
        gtk_window_present (wins->data);
}

gint
main (gint argc, gchar *argv[])
{
    G_PASTE_INIT_APPLICATION_FULL ("Ui", show_win);
    G_PASTE_CLEANUP_ERROR_FREE GError *e = NULL;
    G_PASTE_CLEANUP_UNREF GPasteClient *client = g_paste_client_new_sync (&e);
    G_PASTE_CLEANUP_FREE gpointer *action_data = g_malloc (2 * sizeof (gpointer));

    if (!client)
    {
        g_critical ("%s: %s\n", _("Couldn't connect to GPaste daemon"), (e) ? e->message : "unknown error");
        exit (EXIT_FAILURE);
    }

    action_data[0] = app;
    action_data[1] = client;

    GActionEntry app_entries[] = {
        { "about", about_activated, NULL, NULL, NULL, { 0 } },
        { "quit",  quit_activated,  NULL, NULL, NULL, { 0 } }
    };
    g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries, G_N_ELEMENTS (app_entries), (gpointer) action_data);

    GMenu *menu = g_menu_new ();
    g_menu_append (menu, "About GPaste", "app.about");
    g_menu_append (menu, "Quit", "app.quit");
    gtk_application_set_app_menu (app, G_MENU_MODEL (menu));

    GtkWidget *window = g_paste_ui_window_new (app);

    if (!window)
        exit (EXIT_FAILURE);

    gtk_widget_show_all (window);

    return g_application_run (gapp, argc, argv);
}
