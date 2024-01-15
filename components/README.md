### Components development guide
- Create bash script with extension .component
- Place in some structural way after this dir.
- Take into account that sort order of the file will affect on the severity of component. Used to get dependencies
- Must have function to implement:
```bash
function do_init
{
    # Component initialization (ie build variables basing on config values)
    return
}

function do_sync
{
    # Syncronization. Get sources or binaries. Any network activities must be here
    return
}

function do_build
{
    # Build process. Utilize YALDA_NPROC variable
    return
}

function do_clean
{
    # Complete cleaning of build and sync steps
    return
}
```
- Global variables and functions are completly global. The best way is to prefix them with "@component name@_"
