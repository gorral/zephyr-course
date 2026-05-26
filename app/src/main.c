#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define LED_SENSOR_NODE DT_NODELABEL(led_sensor0)

#if !DT_NODE_HAS_STATUS(LED_SENSOR_NODE, okay)
#error "led_sensor0 node is not defined in devicetree"
#endif

static const struct device *const led_sensor = DEVICE_DT_GET(LED_SENSOR_NODE);

static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
int ret;

ARG_UNUSED(argc);
ARG_UNUSED(argv);

if (!device_is_ready(led_sensor)) {
shell_error(sh, "LED sensor device is not ready");
return -ENODEV;
}

ret = sensor_sample_fetch(led_sensor);
if (ret < 0) {
shell_error(sh, "sensor_sample_fetch failed: %d", ret);
return ret;
}

shell_print(sh, "sensor_sample_fetch executed: LED ON");

return 0;
}

static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
struct sensor_value value;
int ret;

ARG_UNUSED(argc);
ARG_UNUSED(argv);

if (!device_is_ready(led_sensor)) {
shell_error(sh, "LED sensor device is not ready");
return -ENODEV;
}

ret = sensor_channel_get(led_sensor, SENSOR_CHAN_ALL, &value);
if (ret < 0) {
shell_error(sh, "sensor_channel_get failed: %d", ret);
return ret;
}

shell_print(sh, "sensor_channel_get executed: LED OFF");
shell_print(sh, "sensor value: val1=%d, val2=%d", value.val1, value.val2);

return 0;
}

static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
ARG_UNUSED(argc);
ARG_UNUSED(argv);

shell_print(sh, "device name: %s", led_sensor->name);
shell_print(sh, "ready state: %s", device_is_ready(led_sensor) ? "ready" : "not ready");

return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_subcommands,
SHELL_CMD(fetch, NULL, "Call sensor_sample_fetch() and turn LED on.", cmd_sensor_fetch),
SHELL_CMD(read, NULL, "Call sensor_channel_get() and turn LED off.", cmd_sensor_read),
SHELL_CMD(info, NULL, "Print sensor device name and ready state.", cmd_sensor_info),
SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sensor_subcommands, "LED sensor shell commands", NULL);

int main(void)
{
if (!device_is_ready(led_sensor)) {
LOG_ERR("LED sensor device is not ready");
return 0;
}

LOG_INF("LED sensor shell application started");
LOG_INF("Try shell commands: sensor info, sensor fetch, sensor read");

return 0;
}
