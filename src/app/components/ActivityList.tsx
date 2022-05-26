import React from 'react';
import "../styles/styles.css"
import {Activity} from "../models/activity";
import ActivityItem from "./ActivityItem";
import {useStore} from "../stores/Store";
import {observer} from "mobx-react-lite";


function ActivityList() {

    const {activityStore} = useStore();
    const {activitiesByDates} = activityStore;

    return(
        <div className="activityList">
            {activitiesByDates.map((activity)=>(
                <ActivityItem
                    key={activity.id}
                    activity={activity}
                />
            ))}
        </div>
    )
}

export default observer(ActivityList);