import React, {useEffect} from 'react';
import "../styles/styles.css"
import {Activity} from "../models/activity";
import {Link, useParams} from 'react-router-dom'
import Loading from "./Loading";
import {observer} from "mobx-react-lite";
import {useStore} from "../stores/Store";

function ActivityDetails() {

    const {activityStore} = useStore();
    const {selectedActivity:activity, loadActivity, loadingInitial} = activityStore;
    const {id} = useParams<{id:string}>();

    useEffect(()=>{
        if(id) loadActivity(id);
    }, [id, loadActivity])

    if(loadingInitial || !activity)
        return (<Loading/>);

    return(
        <div className="activity_details">
            <div className="activity_item--title">{activity.title}</div>
            <div className="activity_item--date">{activity.date}</div>
            <br/>
            <div className="activity_item--desc">{activity.description}</div>
            <br/>
            <div className="activity_item--place">{activity.city}, {activity.venue}</div>
            <div className="activity_item--category">{activity.category}</div>
            <div className="activity_item--footer">
                <Link to={`/manage/${activity.id}`}>Edit</Link>
                <Link to='/activities'>Cancel</Link>
            </div>
        </div>
)
}

export default observer(ActivityDetails);