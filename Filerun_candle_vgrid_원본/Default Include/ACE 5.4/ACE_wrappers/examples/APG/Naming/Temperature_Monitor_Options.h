// Temperature_Monitor_Options.h,v 1.1 2004/01/01 21:01:00 shuston Exp

#ifndef TEMPERATURE_MONITOR_OPTIONS_H
#define TEMPERATURE_MONITOR_OPTIONS_H

class Temperature_Monitor_Options
  {
  public:
    Temperature_Monitor_Options (int, char *[])
    { }

    const char *thermometer_address (void)
    {
      return "serial:// s0/0x3e52";
    }

    int poll_interval (void)
    {
      return 10; // every 10 seconds
    }

    int reset_interval (void)
    {
      return 60; // sixty seconds
    }

    int excessive_resets (void)
    {
      return 5; // no response in 5 minutes
    }

    const char *admin_email (void)
    {
      return "root@localhost";
    }

    const char *email_from (void)
    {
      return "temperature monitor";
    }

    int history_size()
    {
      return 10;
    }
  };

#endif /* TEMPERATURE_MONITOR_OPTIONS_H */
