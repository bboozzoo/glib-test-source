#include <glib.h>

struct loop_and_int
{
    GMainLoop *l;
    int *cnt;
};

struct loop_and_loop
{
    GMainLoop *th_loop;
    GMainLoop *main_loop;
};

gboolean idle_quit_main_cb(gpointer data);


gboolean timeout_cb(gpointer data)
{
    struct loop_and_loop *ll = (struct loop_and_loop *) data;
    GMainLoop *thread_main = (GMainLoop *) ll->th_loop;

    g_message("(thread timeout cb) timeout source callback in thread: %p", g_thread_self());

    /* push idle to default context handled in main main loop */
    g_idle_add(idle_quit_main_cb, ll->main_loop);

    /* quit self loop */
    g_message("(thread timeout cb) quit self loop %p", thread_main);
    g_main_loop_quit(thread_main);

    return FALSE;
}


gpointer thread_worker(gpointer data)
{
    GMainContext *ctx;
    GMainLoop *thread_main;
    GMainLoop *main_loop = (GMainLoop *) data;
    struct loop_and_loop ll;

    g_message("(thread) in thread: %p", g_thread_self());

    g_message("(thread) default context: %p", g_main_context_default());
    g_message("(thread) thread default context: %p", g_main_context_get_thread_default());

    /* g_message("(thread) default context: %p", g_main_context_default()); */
    /* g_message("(thread) default context: %p", g_main_context_default()); */

    ctx = g_main_context_new();

    thread_main = g_main_loop_new(ctx, FALSE);

    g_message("(thread) set thread default context to: %p", ctx);
    g_main_context_push_thread_default(ctx);
    g_message("(thread) thread default context: %p", g_main_context_get_thread_default());

    ll.th_loop = thread_main;
    ll.main_loop = main_loop;

    g_message("(thread) creating timeout source");
    GSource *timeout = g_timeout_source_new(5000);
    g_source_set_callback(timeout, timeout_cb, &ll, NULL);

    g_source_attach(timeout, ctx);

    g_main_loop_run(thread_main);
    g_message("(thread) loop quit, thread exit");

    g_main_context_unref(ctx);
    g_main_loop_unref(thread_main);

    return NULL;
}


gboolean idle_quit_main_cb(gpointer data)
{
    GMainLoop *l = (GMainLoop *) data;

    g_message("(main idle cb) in thread: %p", g_thread_self());

    g_message("(main idle cb) quit loop");
    g_main_loop_quit(l);

    return FALSE;
}

gboolean main_timeout_cb(gpointer data)
{
    struct loop_and_int *li = (struct loop_and_int *) data;

    int *cnt = li->cnt;;

    g_message("(main timeout cb) in thread: %p", g_thread_self());

    (*cnt)++;
    if (*cnt > 5)
    {
        /* push idle callback that quits main loop */
        //g_idle_add(idle_quit_main_cb, li->l);
        return FALSE;

    }

    return TRUE;
}

int main(int argc, char *argv[])
{
    GMainLoop *m;
    GThread *t;
    struct loop_and_int li;
    int cnt = 0;

    /* g_thread_init(); */

    g_message("(main) in thread: %p", g_thread_self());

    m = g_main_loop_new(NULL, FALSE);

    g_message("(main) default context: %p", g_main_context_default());

    li.l = m;
    li.cnt = &cnt;

    g_message("(main) add timeout");
    g_timeout_add_seconds(2, main_timeout_cb, &li);

    g_message("(main) create  thread");
    t = g_thread_new("other-thread", thread_worker, m);
    g_message("(main) got thread: %p", t);

    g_message("(main) enter loop");
    g_main_loop_run(m);

    g_message("(main) join thread");
    g_thread_join(t);

    g_main_loop_unref(m);
    return 0;
}
