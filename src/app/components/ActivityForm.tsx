import React, {ChangeEvent, useEffect} from 'react';
import {observer} from "mobx-react-lite";
import {useHistory, useParams} from "react-router-dom";
import Loading from "./Loading";
import {useStore} from "../stores/Store";
import {Link} from 'react-router-dom';
import {v4 as uuid} from 'uuid';

function ActivityForm(){

    const history = useHistory();
    const {activityStore} = useStore();
    const {createActivity, updateActivity, loadActivity, loadingInitial} = activityStore;
    const {id} = useParams<{id:string}>();

    const initialActivity = {
        id: '',
        title: '',
        city: '',
        date: '',
        venue:'',
        category:'',
        description:''
    }

    const [activity, setActivity] = React.useState(initialActivity);

    useEffect(()=>{
        if(id)
            loadActivity(id).then(activity=>setActivity(activity!));
    }, [id,loadActivity])

    function handleSubmit(){
        if(activity.id.length === 0){
            let newActivity = {
                ...activity,
                id: uuid()
            };
            createActivity(newActivity).then(()=>history.push(`/activities/${newActivity.id}`));
        }
        else{
            updateActivity(activity).then(()=>history.push((`/activities/${activity.id}`)));
        }
    }

    function handleInputChange(event: ChangeEvent<HTMLInputElement | HTMLTextAreaElement>){
        const {name, value} = event.target;
        setActivity({...activity,[name]:value})
    }

    if(loadingInitial)
        return <Loading/>


    return(
        <form className='create-activity-form'  autoComplete='off'>
            <div className='container'>
            <input placeholder='Title'  name='title' value={activity.title} onChange={handleInputChange}/>
            <textarea placeholder='Description'  name='description' value={activity.description} onChange={handleInputChange} />
            <input placeholder='Category' name='category' value={activity.category} onChange={handleInputChange}/>
            <input type='date' placeholder='Date'  name='date' value={activity.date} onChange={handleInputChange}/>
            <input placeholder='Venue' name='venue' value={activity.venue} onChange={handleInputChange}/>
            <input placeholder='City' name='city' value={activity.city} onChange={handleInputChange}/>
            <Link to='/activities'>Cancel</Link>
            <button type='button' onClick={handleSubmit}>Submit</button>
            </div>
        </form>

    )

}

export default observer(ActivityForm);