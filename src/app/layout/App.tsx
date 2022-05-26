import React from 'react';
import NavBar from "../components/NavBar";
import ActivityDashboard from "../components/ActivityDashboard";
import {observer} from "mobx-react-lite";
import {Route, useLocation} from "react-router-dom";
import HomePage from "../../features/home/HomePage";
import ActivityItem from "../components/ActivityItem";
import ActivityDetails from "../components/ActivityDetails";
import ActivityForm from "../components/ActivityForm";

function App() {

    const location = useLocation();

  return (
    <div className="App">

        <Route exact path='/' component={HomePage}/>
        <Route
            path={'/(.+)'}
            render={()=>(
                <>
                    <NavBar/>
                    <main>

                        <Route exact path='/activities' component={ActivityDashboard}/>
                        <Route path='/activities/:id' component={ActivityDetails}/>
                        <Route key={location.key} path={['/createActivity', '/manage/:id']} component={ActivityForm}/>
                    </main>
                </>
            )}
        />


    </div>
  );
}

export default observer(App);
