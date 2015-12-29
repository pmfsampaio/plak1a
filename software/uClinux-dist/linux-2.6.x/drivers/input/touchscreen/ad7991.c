/*
 * ad7991.c
 *
 *  Created on: Dec 11, 2009
 *      Author: psampaio
 */
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>


static int ad7991_probe(struct i2c_client *client, const struct i2c_device_id *id)
 {
 printk("tsc2007_probe\n");
 return(0);
 }


static int ad7991_remove(struct i2c_client *client)
 {
 printk("ad7991_remove\n");
 return 0;
 }


static struct i2c_driver ad7991_driver = {
 .driver = {
  .name = "ad7991",
 },

 /* iff driver uses driver model ("new style") binding model: */
 .probe = ad7991_probe,
 .remove = ad7991_remove,
};


static int __init ad7991_init(void)
{
 return i2c_add_driver(&ad7991_driver);
}

static void __exit ad7991_exit(void)
{
 i2c_del_driver(&ad7991_driver);
}

module_init(ad7991_init);
module_exit(ad7991_exit);
