// Copyright 2024 MACKI, Krzysztof Gliwiński

typedef enum {
  VALVE_INSTANCE_0 = 0,
  VALVE_INSTANCE_1,
  VALVE_INSTANCE_2,
  VALVE_INSTANCE_3,
  VALVE_INSTANCE_MAX
} valve_instance_t;

void init_solenoid_pins();

void solenoid_open(valve_instance_t valve);

void solenoid_close(valve_instance_t valve);
