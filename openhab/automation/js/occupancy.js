rules.JSRule({
  name: "Door Counter State Aggregator JS",
  description: "Updates overall room occupancy based on door events",
  triggers: [
    triggers.ItemStateUpdateTrigger("Door1_Change_Item"),
    triggers.ItemStateUpdateTrigger("Door2_Change_Item")
  ],
  execute: (event) => {
    let changeVal = parseInt(event.itemState.toString()) || 0;
    let currentItem = items.getItem("Total_Room_Occupancy");
    let currentCount = parseInt(currentItem.state.toString()) || 0;

    let newCount = currentCount + changeVal;
    if (newCount < 0) {
      newCount = 0;
    }

    console.info("[JS Rule] Door update. Change: " + changeVal + " -> New Total: " + newCount);
    currentItem.postUpdate(newCount);
  }
});

rules.JSRule({
  name: "Reset Occupancy Switch JS",
  triggers: [triggers.ItemCommandTrigger("Reset_Occupancy_Switch", "ON")],
  execute: () => {
    console.info("[JS Rule] Manual reset triggered via Switch.");
    items.getItem("Total_Room_Occupancy").postUpdate(0);
    items.getItem("Reset_Occupancy_Switch").postUpdate("OFF");
  }
});

rules.JSRule({
  name: "Manual Occupancy Override JS",
  triggers: [triggers.ItemCommandTrigger("Manual_Occupancy_Override")],
  execute: (event) => {
    let overrideVal = parseInt(event.receivedCommand.toString()) || 0;
    console.info("[JS Rule] Manual override set count to: " + overrideVal);
    items.getItem("Total_Room_Occupancy").postUpdate(overrideVal);
  }
});
