#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/delay.h>

#define CTI_ACK_OFFSET      0x010

/**
 *  Hardcoded offsets of ETM from CTI, only valid for ZynqMP boards 
 */
#define ETM_OFF_FROM_CTI    0x20000
#define ETM_TRCSEQSTR       0x11C

#define THROTTLE_COUNT          0
#define THROTTLE_BREAK_VALUE    3 /* For ETM^2 PR regulator */

struct cti_drvdata {
    void __iomem *base;
    int irq;
    int affinity_cpu; /* -1 if not set */
};

static irqreturn_t cti_irq_handler(int irq, void *dev_id)
{
    struct cti_drvdata *d = dev_id;
    void __iomem *base = d->base;

    /* We wait for:
	 * - either the throttle count
	 * OR
	 * - the ETM state machine changing its state from throttling
	 *
	 * NOTE: we must acknowledge the interrupt afterwards
	 * */
    for (unsigned int i = THROTTLE_COUNT; i > 0; i--) {
        /* readl should provide a memory barrier */
		unsigned int val = readl(base + ETM_OFF_FROM_CTI + ETM_TRCSEQSTR);

        if (val < THROTTLE_BREAK_VALUE) {
            break;
        }
	}

    /* Clear the interrupt on the CTI ACK register*/
    writel((0x1u << 2), base + CTI_ACK_OFFSET);

    return IRQ_HANDLED;
}

static int cti_remove(struct platform_device *pdev)
{
    struct cti_drvdata *d = platform_get_drvdata(pdev);

    if (d && d->irq >= 0)
        irq_set_affinity_hint(d->irq, NULL);

    return 0;
}

static int cti_probe(struct platform_device *pdev)
{
    int irq, ret;
    struct resource *res;
    struct cti_drvdata *d;
    void __iomem *base;
    u32 cpu;

    irq = platform_get_irq(pdev, 0);
    if (irq < 0)
        return irq;

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "missing mem resource\n");
        return -ENODEV;
    }

    base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(base))
        return PTR_ERR(base);

    d = devm_kzalloc(&pdev->dev, sizeof(*d), GFP_KERNEL);
    if (!d)
        return -ENOMEM;

    d->base = base;
    d->irq = irq;
    d->affinity_cpu = -1;
    platform_set_drvdata(pdev, d);

    pr_info("Requesting IRQ %d for CTI\n", irq);

    ret = devm_request_irq(&pdev->dev, irq, cti_irq_handler, 0,
                   dev_name(&pdev->dev), d);
    if (ret) {
        dev_err(&pdev->dev, "Failed to request IRQ %d\n", irq);
        return ret;
    }

    /* Get DT property: irq-affinity = <cpu>; */
    if (!of_property_read_u32(pdev->dev.of_node, "irq-affinity", &cpu)) {
        if (cpu < nr_cpu_ids) {
            irq_set_affinity_hint(irq, cpumask_of(cpu));
            d->affinity_cpu = cpu;
            dev_info(&pdev->dev, "requested IRQ %d affinity to CPU %u\n",
                 irq, cpu);
        } else {
            dev_warn(&pdev->dev, "irq-affinity %u out of range\n", cpu);
        }
    }

    return 0;
}

static const struct of_device_id cti_of_match[] = {
    { .compatible = "arm,coresight-cti" },
    { }
};
MODULE_DEVICE_TABLE(of, cti_of_match);

static struct platform_driver cti_driver = {
    .probe = cti_probe,
    .remove = cti_remove,
    .driver = {
        .name = "etm-reg-cti",
        .of_match_table = cti_of_match,
    },
};

module_platform_driver(cti_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ashutosh Pradhan, Daniele Ottaviano");
MODULE_DESCRIPTION("CTI Driver for ETM^2 (for ZynqMP platforms)");
