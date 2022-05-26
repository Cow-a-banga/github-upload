import React from 'react';
import "../styles/styles.css"
import {NavLink} from "react-router-dom";
import {useStore} from "../stores/Store";

export default function NavBar() {

    const {activityStore} = useStore();

    return(
        <div className="navbar">
            <div className="container">
                <NavLink to='/' className="navbar--logo navbar--item">
                    <img className="navbar--logo_img" src="/assets/logo.png" alt="logo"/>
                    Reactivities
                </NavLink>
                <NavLink to='/activities' className="navbar--activities navbar--item" >Activities</NavLink>
                <NavLink to='/createActivity' className="navbar--create_activity navbar--item" >Create Activity</NavLink>
            </div>
        </div>
    )
}