import React, {useEffect} from 'react';
import "../styles/styles.css"
import {Activity} from "../models/activity";
import ActivityList from "./ActivityList";
import Loading from "./Loading";
import {observer} from "mobx-react-lite";
import {intercept} from "mobx";
import ActivityDetails from "./ActivityDetails";
import ActivityForm from "./ActivityForm";
import {useStore} from "../stores/Store";

function ActivityDashboard() {

    const {activityStore} = useStore();
    const {loadActivities, activityRegistry} = activityStore;

    useEffect(()=>{
        if(activityRegistry.size <= 1)
            loadActivities();
    },[activityRegistry.size, loadActivities])


    if(activityStore.loadingInitial)
        return (
            <Loading/>
        )

    return(
        <div className="dashboard">
            <div className="container">
                <ActivityList />
            </div>
        </div>
    )
}

export default observer(ActivityDashboard);