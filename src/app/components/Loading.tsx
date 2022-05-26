import React from 'react';
import "../styles/styles.css";

interface Props{
    inverted?: boolean;
    content?:string;
}

export default function Loading({inverted,content}:Props) {

    return(
    <div className="loading">
        Loading...
    </div>
    )
}