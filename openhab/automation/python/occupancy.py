from openhab import rule
from openhab.triggers import ItemStateUpdateTrigger, ItemCommandTrigger
from openhab.log import logging

logger = logging.getLogger("org.openhab.core.automation.occupancy")

@rule("Door Counter State Aggregator Python")
@when("Item Door1_Change_Item received update")
@when("Item Door2_Change_Item received update")
def process_door_update(event):
    try:
        change_val = int(float(str(event.itemState)))
    except (ValueError, TypeError):
        change_val = 0

    total_item = items.getItem("Total_Room_Occupancy")
    try:
        current_count = int(float(str(total_item.state)))
    except (ValueError, TypeError):
        current_count = 0

    new_count = current_count + change_val
    if new_count < 0:
        new_count = 0

    logger.info(f"[Python Rule] Door update received. Change: {change_val} -> New Total: {new_count}")
    events.postUpdate("Total_Room_Occupancy", str(new_count))


@rule("Reset Occupancy Switch Python")
@when("Item Reset_Occupancy_Switch received command ON")
def process_reset(event):
    logger.info("[Python Rule] Manual reset triggered via Switch.")
    events.postUpdate("Total_Room_Occupancy", "0")
    events.postUpdate("Reset_Occupancy_Switch", "OFF")


@rule("Manual Occupancy Override Python")
@when("Item Manual_Occupancy_Override received command")
def process_override(event):
    try:
        override_val = int(float(str(event.receivedCommand)))
    except (ValueError, TypeError):
        override_val = 0

    logger.info(f"[Python Rule] Manual override set count to: {override_val}")
    events.postUpdate("Total_Room_Occupancy", str(override_val))
