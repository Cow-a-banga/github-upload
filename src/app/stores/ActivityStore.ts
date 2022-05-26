import {makeAutoObservable, makeObservable, observable, runInAction} from "mobx";
import {Activity} from "../models/activity";
import agent from "../api/agent";

export default class ActivityStore{
    activityRegistry  = new Map <string,Activity> ();
    selectedActivity :Activity | undefined = undefined;
    editMode = false;
    loading = false;
    loadingInitial = false;

    constructor() {
        makeAutoObservable(this);
    }
    
    get activitiesByDates(){
        return Array.from(this.activityRegistry.values())
            .sort((a,b)=>Date.parse(a.date) - Date.parse(b.date));
    }

    createActivity = async (activity:Activity) => {
        this.loading = true;
        try{
            await agent.Activities.create(activity);
            runInAction(()=>{
                this.activityRegistry.set(activity.id, activity);
                this.selectedActivity = activity;
                this.editMode = false;
                this.loading = false;
            })
        }catch (e) {
            console.log(e);
            runInAction(()=>{
                this.loading = false;
            })
        }
    }

    updateActivity = async (activity:Activity) => {
        this.loading = true;
        try{
            await agent.Activities.update(activity);
            runInAction(()=>{
                this.activityRegistry.set(activity.id, activity);
                this.selectedActivity = activity;
                this.editMode = false;
                this.loading = false;
            })
        }catch (e) {
            console.log(e);
            runInAction(()=>{
                this.loading = false;
            })
        }
    }

    deleteActivity = async (id:string) => {
        this.loading = true;
        try{
            await agent.Activities.delete(id);
            runInAction(()=>{
                this.activityRegistry.delete(id);
                this.loading = false;
            })
        }catch (e) {
            console.log(e);
            runInAction(()=>{
                this.loading = false;
            })
        }
    }

    private getActivity = (id:string) => {
        return this.activityRegistry.get(id);
    }

    private setActivity = (a:Activity) => {
        a.date = a.date.split('T')[0];
        this.activityRegistry.set(a.id,a);
    }

    private setLoadingInitial = (value:boolean) => {
        this.loadingInitial = value;
    }

    loadActivity = async (id:string) => {
        let activity = this.getActivity(id);

        if(activity){
            this.selectedActivity = activity;
            return activity;
        }
        else{
            this.loadingInitial = true;
            try{
                activity = await agent.Activities.details(id);
                this.setActivity(activity);
                this.selectedActivity = activity;
                this.setLoadingInitial(false);
                return activity;
            }
            catch (e) {
                console.log(e);
                this.setLoadingInitial(false);
            }
        }
    }

    loadActivities = async () => {
        this.loadingInitial = true;
        try{
            const activities = await agent.Activities.list();
            runInAction(()=>{
                activities.forEach(a=>{
                    this.setActivity(a);
                })
                this.loadingInitial = false;
            })
        }catch(e){
            console.log(e);
            runInAction(()=>{
                this.loadingInitial = false;
            })

        }
    }


}