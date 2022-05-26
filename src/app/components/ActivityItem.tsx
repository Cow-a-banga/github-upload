import React from 'react';
import "../styles/styles.css"
import {Activity} from "../models/activity";
import {Link} from 'react-router-dom'
import {useStore} from "../stores/Store";

interface Props{
    activity: Activity
}

export default function ActivityItem({activity}:Props) {

    const {activityStore} = useStore();
    const {deleteActivity} = activityStore;

    return(
        <div className="activity_item">
            <div className="activity_item--title">{activity.title}</div>
            <div className="activity_item--date">{activity.date}</div>
            <br/>
            <div className="activity_item--desc">{activity.description}</div>
            <br/>
            <div className="activity_item--place">{activity.city}, {activity.venue}</div>
            <div className="activity_item--footer">
                <div className="activity_item--category">{activity.category}</div>
                <Link to={`/activities/${activity.id}`} className="activity_item--view_button">View</Link>
                <a onClick={()=>deleteActivity(activity.id)} className="activity_item--delete_button">Delete</a>
            </div>
        </div>
    )
}