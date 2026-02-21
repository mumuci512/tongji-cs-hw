if( context->xcs & USER_MODE ) /*先前为用户态*/
{
    while(true)
    {

        X86Assembly::CLI(); /* 关中断 */

        if(Kernel::Instance().GetProcessManager().RunRun > 0)
        {

            X86Assembly::STI(); /* 开中断 */

            Kernel::Instance().GetProcessManager().Swtch(); /* 进行进程切换调度 */

        }
        else
        {

            break; /* 如果runrun == 0，则退栈回到用户态继续用户程序的执行 */

        }

    }

}