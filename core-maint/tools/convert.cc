#include "convert.h"

void c(gm_graph& G)
{
    //Initializations
    gm_rt_initialize();
    G.freeze();

}


#define GM_DEFINE_USER_MAIN 1
#if GM_DEFINE_USER_MAIN

// convert -? : for how to run generated main program
int main(int argc, char** argv)
{
    gm_default_usermain Main;


    if (!Main.process_arguments(argc, argv)) {
        return EXIT_FAILURE;
    }

    if (!Main.do_preprocess()) {
        return EXIT_FAILURE;
    }

    Main.begin_usermain();
    c(
        Main.get_graph());
    Main.end_usermain();

    if (!Main.do_postprocess()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
#endif
